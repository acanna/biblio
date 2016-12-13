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
    static void my_push(ArticleInfo info, std::vector<ArticleInfo> &out);
    static std::string my_pop(std::queue<std::string> &in);
    static bool my_empty(std::queue<std::string> &in);

    static bool greater(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static bool smaller(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static bool longer_title(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static void thread_function(std::function<size_t(const std::string &, const std::string &)> dist, bool offline,
                                std::queue<std::string> &in, std::vector<ArticleInfo> &out);
    static std::vector<ArticleInfo> search_requester(Requester &requester, std::string query);

public:
    BiblioManager();
    BiblioManager(int threads);
    std::vector<ArticleInfo> search_distance(std::function<size_t(const std::string &,
                                                                  const std::string &)> dist,
                                             const std::vector<std::string> &filenames, bool offline);
    static void print_html(std::ostream &out, std::vector<ArticleInfo> &result);
    void print_bib(std::ostream &out, std::vector<ArticleInfo> &result);
};
