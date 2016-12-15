#pragma once

#include <mutex>
#include <queue>
#include "ArticleInfo.h"

class BiblioThreadContext {
private:
    static size_t size;
    static size_t count;
    static std::queue<std::string> in;
    static std::vector<ArticleInfo> out;

    BiblioThreadContext() {};
    ~BiblioThreadContext() {};
    BiblioThreadContext(BiblioThreadContext const &);
    BiblioThreadContext& operator= (BiblioThreadContext const &);

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

