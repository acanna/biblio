#pragma once

#include <mutex>
#include <queue>

#include "ArticleInfo.h"

class BiblioThreadContext {
private:
    std::mutex m_empty;
    std::mutex m_push;
    std::mutex m_pop;
    static size_t size;
    static size_t count;
    static std::queue<std::string> in;
    static std::vector<ArticleInfo> out;

    BiblioThreadContext() {};
    ~BiblioThreadContext() {};
    BiblioThreadContext(BiblioThreadContext const &) = delete;
    BiblioThreadContext& operator= (BiblioThreadContext const &) = delete;

public:
    static void init(std::queue<std::string> &_in) {
        in = _in;
        count = 0;
        size = in.size();
    }
    static BiblioThreadContext& instance() {
        static BiblioThreadContext biblio;
        return biblio;
    };
    void my_push(ArticleInfo info);
    std::string my_pop();
    bool my_empty();
    std::vector<ArticleInfo> get_output() {return out;};
};

