#ifndef BIBLIO_BIBLIOMANAGER_H
#define BIBLIO_BIBLIOMANAGER_H

#include <functional>
#include "Parser.h"
#include "Biblio_exception.h"
#include "DBLPRequester.h"
#include "SpringerRequester.h"
#include "PictureParser.h"

class BiblioManager {

private:
    Parser  parser;
    PictureParser picture_parser;

private:
    static bool greater(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static bool smaller(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static bool longer_title(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static void thread_search_function(int i, std::vector<std::string> &title_candidates, std::vector<std::vector<ArticleInfo>> &results);

public:
    BiblioManager() {};
//    BiblioManager(const std::string &filename);
    std::vector<ArticleInfo> search_requester(Requester& requester, std::string query);
    std::vector<ArticleInfo> search_distance_requesters(std::vector<Requester*> requesters, std::function<size_t(const std::string &, const std::string &)> dist,
                                                  const std::string &filename, bool offline);
    static void print_html(std::ostream &out, const std::string &filename, std::vector<ArticleInfo> &result);
    void print_bib(std::ostream &out, std::vector<ArticleInfo> &result);
    void print_txt(std::ostream &out, const std::string &filename, std::vector<ArticleInfo> &result);
};


#endif
