#pragma once

#include <functional>
#include <mutex>
#include <queue>
#include "Parser.h"
#include "Biblio_exception.h"
#include "Requester.h"
#include "PictureParser.h"

class BiblioManager {

private:
    int threads_num;
    Parser  parser;
    PictureParser picture_parser;

private:
    static void my_push(ArticleInfo info, std::vector<ArticleInfo> &out);
    static std::string my_pop(std::queue<std::string> &in);
    static bool my_empty(std::queue<std::string> &in);
    static bool greater(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static bool smaller(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static bool longer_title(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static void thread_function(std::vector<Requester *> requesters,
                                std::function<size_t(const std::string &, const std::string &)> dist,
                                bool offline, std::queue<std::string> &in, std::vector<ArticleInfo> &out);
    static void thread_function_data(std::vector<std::pair<requestersEnum, std::vector<std::string>>> data,
                                std::function<size_t(const std::string &, const std::string &)> dist,
                                bool offline, std::queue<std::string> &in, std::vector<ArticleInfo> &out);
    static std::vector<ArticleInfo> search_requester(Requester &requester, std::string query);

public:
    BiblioManager() {};
    BiblioManager(int threads);
    std::vector<ArticleInfo> search_distance_requesters(std::vector<Requester *> requesters,
                                                        std::function<size_t(const std::string &,
                                                                             const std::string &)> dist,
                                                        const std::vector<std::string> &filenames, bool offline);
    std::vector<ArticleInfo> search_distance_data(std::vector<std::pair<requestersEnum, std::vector<std::string>>> data,
                                                  std::function<size_t(const std::string &,
                                                                       const std::string &)> dist,
                                                  const std::vector<std::string> &filenames, bool offline);
    static void print_html(std::ostream &out, std::vector<ArticleInfo> &result);
    void print_bib(std::ostream &out, std::vector<ArticleInfo> &result);
    void print_txt(std::ostream &out, const std::string &filename, ArticleInfo &result);
};
