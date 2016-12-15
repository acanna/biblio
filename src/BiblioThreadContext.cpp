#include <iostream>

#include "BiblioThreadContext.h"

size_t BiblioThreadContext::size;
size_t BiblioThreadContext::count;
std::queue<std::string> BiblioThreadContext::in;
std::vector<ArticleInfo> BiblioThreadContext::out;

void BiblioThreadContext::my_push(ArticleInfo info) {
    out.push_back(info);
}

std::string BiblioThreadContext::my_pop() {
    std::string name;
    name = in.front();
    in.pop();
    count++;
    std::cout << "Processing file " << count << " from " << size << ": " << name << std::endl;
    return name;
}

bool BiblioThreadContext::my_empty() {
    return in.empty();
}
