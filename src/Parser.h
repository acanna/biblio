#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <list>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

class Parser{
	poppler::document * doc;
	std::vector<std::string> fst_page;
	public:
		Parser(const std::string &);
		std::vector<std::string> get_document() const;
		std::vector<std::string> get_fst_page() const;
		std::list<std::string> get_authors() const;
		std::list<std::string> get_title() const;
};

#endif
