#include "md2html.h"

bool md2html::IsEmptyLine(string & line,int &locate){
	if(locate<line.length()){
		return false;
	}
	else{
		return true;
	}
}

void md2html::Strip(string & line,int &locate){
	for(int i=locate;i<line.length();i++){
		if(line[i]==' ' || line[i] == '\t'){
			continue;
		}
		locate=i;
		return;
	}
	locate=line.length();
}

bool md2html::IsSymbol(string & line,int &locate,string symbol){
	if(locate+symbol.length()>line.length()){
		return false;
	}
	for(int i=0;i<symbol.length();i++){
		if(symbol[i]!=line[locate+i]){
			return false;
		}
	}
	locate+=symbol.length();
	return true;
}

void md2html::CreateNewNode(token one){
	if(current_node->type_==one){
		current_node=current_node->GetFather();
	}
	else{
		current_node->sub_node_.push_back(new node(current_node));
		current_node=current_node->sub_node_.back();
		current_node->type_=one;
	}
}

void md2html::GetNextSymbol(string & line,int &locate){
	if(line[locate]=='['){
		current_node->sub_node_.push_back(new node(current_node));
		current_node=current_node->sub_node_.back();
		if(GetLink(line,locate)){
			current_node->type_=a;
			current_node=current_node->GetFather();
		}
		else{
			current_node->type_=nul;

			int i=locate+1;
			for(i;i<line.length();i++){
				if(symbol_pre.find(line[i])!=symbol_pre.end()){
					break;
				}
			}
			current_node->content_=line.substr(locate,i-locate);
			locate=i;
			current_node=current_node->GetFather();
			//if not a link,pop
			//current_node->sub_node_.pop_back();
			//delete temp;
		}
	}
	else if(IsSymbol(line,locate,"`")){
		CreateNewNode(code);
	}
	else if(IsSymbol(line,locate,"**")){
		CreateNewNode(strong);
	}
	else if(IsSymbol(line,locate,"*")){
		CreateNewNode(italic);
	}
	else if(IsSymbol(line,locate,"~~")){
		CreateNewNode(strike);
	}
	else{
		current_node->sub_node_.push_back(new node(current_node));
		current_node=current_node->sub_node_.back();
		int i=locate;
		for(i;i<line.length();i++){
			if(symbol_pre.find(line[i])!=symbol_pre.end()){
				break;
			}
		}
		current_node->content_=line.substr(locate,i-locate);
		locate=i;
		current_node=current_node->GetFather();
	} 
}

bool md2html::GetLink(string &line,int &locate){
	int l_brackets=0,r_brackets=0,m_brackets=0,blank=0;
	for(int i=locate+1;i<line.length();i++){
		if(line[i]==']'){
			m_brackets=i;
		}
		if(line[i]=='('){
			l_brackets=i;
		}
		if(l_brackets!=0 && line[i]==' '){
			blank=i;
		}
		if(line[i]==')'){
			r_brackets=i;
			break;
		}
	}
	if(l_brackets==0 || r_brackets==0 || m_brackets==0){
		return false;
	}
	if(l_brackets-1!=m_brackets || r_brackets<= l_brackets){
		return false;
	}
	current_node->content_=line.substr(locate+1,m_brackets-locate-1);
	if(blank==0){
		current_node->link_=line.substr(l_brackets+1,r_brackets-l_brackets-1);
	}
	else{
		current_node->link_=line.substr(l_brackets+1,blank-l_brackets-1);
		current_node->title_=line.substr(blank+1,r_brackets-blank-1);
	}
	locate=r_brackets+1;
	return true;
}

void md2html::FindRootSymbol(string & line,int &locate,bool &new_root){
	int length=line.length();
	char begin=line[locate];
	if(begin=='#' && current_root->type_!=pre){
		if(current_root->type_!=nul){
			CreateNewRoot();
		}
		int count=1;
		for(int i=locate;i<line.length();i++){
			if(line[i]=='#'){
				count++;
			}
		}
		/* if all # be hr
		if(count==line.length()){
			symbol=hr;
			return;
		}
		*/
		locate+=count;
		if(count>6){
			count=6;
		}
		current_node->type_=static_cast<token>(nul+count);
		new_root=true;
	}
	else if(begin=='!'){
		locate++;
		if(line[locate]=='['){
			if(GetLink(line,locate)){
				if(current_root->type_!=nul){
					CreateNewRoot();
				}
				current_root->type_=img;
				new_root=true;
				return;
			}
		}
		locate--;
	}
	else if(IsSymbol(line,locate,"---")){
		if(current_root->type_!=nul){
			CreateNewRoot();
		}
		current_node->type_=hr;
		new_root=true;
	}
	else if(isdigit(begin)){
		locate++;
		if(IsSymbol(line,locate,". ")){
			if(current_root->type_==nul){
				current_root->type_=ol;
			}
			else if(current_root->type_!=ol){
				CreateNewRoot();
				current_root->type_=ol;
			}
			current_root->sub_node_.push_back(new node(current_root));
			current_node=current_root->sub_node_.back();
			current_node->type_=li;
			new_root=false;
		}
		else{
			locate--;
		}
	}
	//table
	else if(begin=='|'){
		//new_root=false;
	}
	else if(IsSymbol(line,locate,"```")){
		if(current_root->type_==pre){
			new_root=true;
		}
		else{
			//todo
			if(current_root->type_!=nul){
				CreateNewRoot();
			}
			current_node->type_=pre;
			new_root=false;
		}
	}
	else if(IsSymbol(line,locate,"- ")){
		if(current_root->type_==nul){
			current_root->type_=ul;
		}
		else if(current_root->type_!=ul){
			CreateNewRoot();
			current_root->type_=ul;
		}
		current_root->sub_node_.push_back(new node(current_root));
		current_node=current_root->sub_node_.back();
		current_node->type_=li;
		new_root=false;
	}
	else if(IsSymbol(line,locate,"> ")){
		if(current_root->type_==nul){
			current_node->type_=quote;
			new_root=false;
		}
	}
	else{
		current_node->sub_node_.push_back(new node(current_node));
		current_node=current_node->sub_node_.back();
		current_node->type_=para;
	}

	if(current_node->type_==nul){
		current_node->type_=para;
	}
}

void md2html::CreateNewRoot(){
	tree_.push_back(new node(NULL));
	current_root=tree_.back();
	current_node=current_root;
}

void md2html::GenerateTree(vector<string> & input){
	int locate;
	bool new_root=true;
	for(auto &line : input){
		locate=0;
		Strip(line,locate);
		//if the line is empty,continue
		if(IsEmptyLine(line,locate)){
			tree_.push_back(new node(NULL));
			//tree_.back()->type_=br;
			if(current_root->type_!=pre){
				new_root=true;
			}
			continue;
		}
		if(new_root){
			CreateNewRoot();
		}
		FindRootSymbol(line,locate,new_root);
		while(locate<line.length()){
			GetNextSymbol(line,locate);
		}
	}
}

void md2html::Dfs(node * root){
	if(root->sub_node_.size()==0){
		if(root->type_==img){
			content+="<img alt="+root->content_+" src="+root->link_ + " title="+root->title_+" />";
			return;
		}
		else{
			//nul
			content+=root->content_;
		}
		return;
	}
	for(auto i : root->sub_node_){
		if(i->type_==a){
			content+="<a href="+i->link_+">"+i->content_+"</a>";
			continue;
		}
		if(i->type_!=nul){
			content+="<"+tag[i->type_]+">";
		}
		Dfs(i);
		if(i->type_!=nul){
			content+="</"+tag[i->type_]+">";
		}
	}

}

void md2html::ToHtml(){
	//dfs to create html
	for(auto one: tree_){
		if(one->type_==hr || one->type_==br){
			content+="<"+tag[one->type_]+" />";
			continue;
		}
		
		if(one->type_!=img && one->type_!=nul){
			content+="<"+tag[one->type_]+">";
		}
		Dfs(one);
		if(one->type_!=img && one->type_!=hr && one->type_!=br && one->type_!=nul){
			content+="</"+tag[one->type_]+">";
		}
		content+="\n";
	}
}
