#ifndef MD_TO_HTML
#define MD_TO_HTML

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

set<char> symbol_pre={'*','`','~','['};

enum token{
	nul		=0,
	h1		=1,
	h2		=2,
	h3		=3,
	h4		=4,
	h5		=5,
	h6		=6,
	ul		=7,
	ol		=8,
	li		=9,
	quote	=10,
	pre		=11,
	a		=12,
	href	=13,
	img		=14,
	src		=15,
	code	=16,
	italic	=17,
	strong	=18,
	strike	=19,
	hr		=20,
	br		=21,
	para	=22,

	table	=23,
	tr		=24,
	td		=25
}

class node{
	public:
		node(node * father){
			type_=nul;
			content_="";
			link_="";
			title_="";
			father_=father;
			is_end=true;
		}
		token type_;
		string content_;//used by nul or img(alt)
		string link_;//link address,used by link(href) and img(src)
		string title_;//used by link and img
		bool is_end;
		vector<node *> sub_node_;

		node * GetFather(){
			return father_;
		}
	private:
		node * father_;
};

class md2html{
	public:
		node * current_node;
		node * current_root;
		vector<node *> tree_;
		md2html::md2html(string &fname){
			vector<string> input;
			ifstream fin(fname);
			while(!fin.eof()){
				input.push_back("");
				auto &one = input.back();
				fin>>one;
			}
			fin.close();

			GenerateTree(input);
		}
		md2html(vector<string> & input){
			GenerateTree(input);
		}
		bool IsEmptyLine(string &line,int &locate);
		void Strip(string &line,int &locate);
		bool IsSymbol(string &line,int & locate);
		void CreateNewNode(token one);
		void CreateNewRoot();
		bool GetLink(string &line,int &locate);
		void GetNextSymbol(string & line,int &locate);
		void FindRootSymbol(string &line,int &locate);
		void GenerateTree(vector<string> &input);
		void ToHtml();
};

#endif
