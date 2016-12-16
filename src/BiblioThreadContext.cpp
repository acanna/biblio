#include <iostream>

#include "BiblioThreadContext.h"

size_t BiblioThreadContext::size;
size_t BiblioThreadContext::count;
std::queue<std::string> BiblioThreadContext::in;
std::vector<ArticleInfo> BiblioThreadContext::out;

void BiblioThreadContext::my_push(ArticleInfo info) {
    m_out.lock();
    out.push_back(info);
    m_out.unlock();
}

std::string BiblioThreadContext::my_pop() {
    std::string name;
    m_in.lock();
	if (in.empty()) {
	    m_in.unlock();
		return "";
	}
    name = in.front();
    in.pop();
    count++;
    std::cout << "Processing file " << count << " from " << size << ": " << name << std::endl;
    m_in.unlock();
    return name;
}

bool BiblioThreadContext::my_empty() {
    bool empty;
    m_in.lock();
    empty = in.empty();
    m_in.unlock();
    return empty;
}
