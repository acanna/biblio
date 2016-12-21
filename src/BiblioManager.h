#pragma once

#include <functional>
#include <mutex>
#include <queue>

#include "BiblioException.h"
#include "Requesters/Requester.h"
#include "PictureParser.h"
#include "tools.h"

class BiblioManager {

private:
    int threads_num;

private:
    static bool greater(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static void thread_function(std::function<size_t(const std::string &, const std::string &)> dist, bool offline);
    static std::vector<ArticleInfo> search_requester(Requester &requester, std::string query);

public:
    BiblioManager();
    BiblioManager(int threads);
    std::vector<ArticleInfo> search_distance(std::function<size_t(const std::string &,
                                              const std::string &)> dist, bool offline);
    static void start_print_html(std::ostream &out);
    static void end_print_html(std::ostream &out);
    static void print_html(std::ostream &out,  std::vector<ArticleInfo> &result);
    static void cout_not_found_articles( std::vector<ArticleInfo> &result);
    void print_bib(std::ostream &out,  std::vector<ArticleInfo> &result);
};
