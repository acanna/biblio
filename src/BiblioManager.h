#ifndef BIBLIO_BIBLIOMANAGER_H
#define BIBLIO_BIBLIOMANAGER_H

#include "Parser.h"
#include "Biblio_exception.h"
#include "DBLPManager.h"


class BiblioManager {

private:

    Parser parser;
    DBLPManager requester;

    static bool greater(const ArticleInfo &info_1, const ArticleInfo &info_2);

    static bool smaller(const ArticleInfo &info_1, const ArticleInfo &info_2);

    static bool longer_title(const ArticleInfo &info_1, const ArticleInfo &info_2);

    static void thread_search_function(std::string s, std::vector<ArticleInfo> &result);

public:

    BiblioManager();

    BiblioManager(std::string &filename);

    static std::vector<ArticleInfo> search_dblp(std::string query);

    static std::vector<ArticleInfo> search_title(const std::string &filename, std::ostream &out);

    std::vector<std::string> get_fst_page();

    std::vector<ArticleInfo> search_exact_match(const std::string &filename, bool offline);

    std::vector<ArticleInfo> search_damerau_levenshtein(const std::string &filename, bool offline);

    std::vector<ArticleInfo> search_levenshtein(std::ostream &out, const std::string &filename, bool offline);

    std::vector<ArticleInfo> search_levenshtein(const std::string &filename, bool offline);

    std::vector<ArticleInfo> search_levenshtein_light(const std::string &filename, bool offline);

    std::vector<ArticleInfo> search_levenshtein_light_threads(const std::string &filename, bool offline);

    static void print_html(std::ostream &out, const std::string &filename, std::vector<ArticleInfo> &result);

    void print_bib(std::ostream &out, std::vector<ArticleInfo> &result);

    void print_txt(std::ostream &out, const std::string &filename, std::vector<ArticleInfo> &result);

};


#endif
