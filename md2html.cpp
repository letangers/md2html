#include "md2html.h"

bool md2html::IsEmptyLine(string & line,int &locate){
	if(locate<line.length()){
		return false;
	}
	else{
		return true;
	}
}

void Strip(string & line,int &locate){
	for(int i=locate;i<line.length();i++){
		if(line[i]==' ' || line[i] == '\t'){
			continue;
		}
		locate=i;
		break;
	}
	locate=line.length();
}

bool IsSymbol(string & line,int &locate,string symbol){
	if(locate+symbol.length()>=line.length()){
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
		current_node=current_node->current_node->sub_node_.back();
		current_node->type_=one;
	}
}

void md2html::GetNextSymbol(string & line,int &locate){
	if(line[locate]=='['){
		current_node->sub_node_.push_back(new node(current_node));
		current_node=current_node->sub_node_.back();
		if(GetLink(line,locate)){
			current_node=current_node->GetFather();
		}
		else{
			//if not a link,pop
			node * temp=current_node->sub_node_.back();
			current_node->sub_node_.pop();
			del temp;
		}
	}
	else if(IsSymbol(line,locate,"`")){
		CreateNewNode(code);
		locate++;
	}
	else if(IsSymbol(line,locate,"**")){
		CreateNewNode(strong);
		locate+=2;
	}
	else if(IsSymbol(line,locate,"*")){
		CreateNewNode(italic);
		locate++;
	}
	else if(IsSymbol(line,locate,"~~")){
		CreateNewNode(strike);
		locate++;
	}
	else{
		current_node->sub_node_.push_back(new node(current_node));
		current_node=current_node->sub_node_.back();
		for(locate;locate<line.length;locate++){
			if(symbol_pre.find(line[locate])!=symbol_pre.end()){
				break;
			}
			current_node->content_+=line[locate];
		}
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
		if(line[i]==' '){
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
	current_node->content_=line.substr(locate+1,m_brackets-locate);
	if(blank==0){
		current_node->link_=line.substr(l_brackets+1,r_brackets-l_brackets);
	}
	else{
		current_node->link_=line.substr(l_brackets+1,blank-l_brackets);
		current_node->title_=line.substr(blank+1,r_brackets-blank);
	}
	locate=r_brackets+1;
	return true;
}

void FindRootSymbol(string & line,int &locate,bool &new_root){
	int length=line.length();
	char begin=line[locate];
	if(begin=='#'){
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
		current_node->type_=nul+count;
		new_root=true;
	}
	else if(begin=='!'){
		locate++;
		if(line[locate]=='['){
			if(GetLink(line,locate)){
				if(current_root->type_!=nul){
					CreateNewRoot();
				}
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
			if(current_root->type_!=ol && current_root->type_!=nul){
				CreateNewRoot();
			}
			current_node->type_=ol;
			CreateNewNode(li);
			locate+=2;
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
		if(current_root->type_!=ul && current_root->type_!=nul){
			CreateNewRoot();
		}
		current_node->type_=ul;
		CreateNewNode(li);
		new_root=false;
	}
	else if(IsSymbol(line,locate,"> ")){
		if(current_root->type_==nul){
			current_node->type_=quote;
			new_root=false;
		}
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
			tree_.back()->type_=br;
			new_root=true;
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

void md2html::ToHtml(){
}
