#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <list>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

using namespace std;

class parser{
	poppler::document * doc;
	vector<string> fst_page;
	//vector<string> prep_lines;
	//bool prep;
	public:
		parser(string);
		vector<string> parse();
		vector<string> get_fst_page();
		//void prep_lns();
		list<string> get_authors();
		list<string> get_title();
		vector<poppler::font_info> get_doc_fonts();
		//string toc_title();
		string get_metadata();
};

#endif
