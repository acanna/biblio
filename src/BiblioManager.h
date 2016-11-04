#ifndef BIBLIO_BIBLIOMANAGER_H
#define BIBLIO_BIBLIOMANAGER_H

#include "Parser.h"
#include "Biblio_exception.h"
#include "DBLPManager.h"


class BiblioManager {

private:

    Parser parser;
    DBLPManager requester;

    std::vector <ArticleInfo> search_dblp (DBLPManager & dblp, std::string & query);

    static bool greater (const ArticleInfo & info_1, const ArticleInfo & info_2);

public:

    BiblioManager();

    std::vector <ArticleInfo> search_exact_match(const std::string & filename, bool offline);

    std::vector <ArticleInfo> find_info(const std::string & filename, bool offline);

    void print_html(std::ostream & out, const std::string & filename, std::vector <ArticleInfo> & result);
    void print_bib(std::ostream & out, std::vector <ArticleInfo> & result);
    void print_txt(std::ostream & out, const std::string & filename, std::vector <ArticleInfo> & result);

};


#endif
