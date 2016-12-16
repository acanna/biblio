#include <iostream>

#include "BiblioThreadContext.h"

size_t BiblioThreadContext::size;
size_t BiblioThreadContext::count;
std::queue<std::string> BiblioThreadContext::in;
std::vector<ArticleInfo> BiblioThreadContext::out;

void BiblioThreadContext::my_push(ArticleInfo info) {
    m_push.lock();
    out.push_back(info);
    m_push.unlock();
}

std::string BiblioThreadContext::my_pop() {
    std::string name;
    m_pop.lock();
	if (in.empty()) {
	    m_pop.unlock();
		return "";
	}
    name = in.front();
    in.pop();
    count++;
    std::cout << "Processing file " << count << " from " << size << ": " << name << std::endl;
    m_pop.unlock();
    return name;
}

bool BiblioThreadContext::my_empty() {
    bool empty;
    m_pop.lock();
    empty = in.empty();
    m_pop.unlock();
    return empty;
}
