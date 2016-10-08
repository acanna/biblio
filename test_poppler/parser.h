#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <list>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

//using namespace std;

class parser{
	poppler::document * doc;
	std::vector<std::string> fst_page;
	//vector<string> prep_lines;
	//bool prep;
	public:
		parser(const std::string &);
		std::vector<std::string> parse() const;
		std::vector<std::string> get_fst_page() const;
		//void prep_lns();
		std::list<std::string> get_authors() ;
		std::list<std::string> get_title() ;
		std::vector<poppler::font_info> get_doc_fonts() const;
		//string toc_title();
		std::string get_metadata() const;
};

#endif
