#ifndef TOOLS_H
#define TOOLS_H

#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> split(const std::string &str, char delimiter);

std::string low_letters_only(std::string str);

std::string delete_multiple_spaces(std::string str);

std::string short_name(std::string s);

std::string delete_junk_symbol(std::string str);

size_t levenshtein_distance(const std::string &s, const std::string &t);

size_t damerauLevenshteinDistance_biblio(const std::string &a, const std::string &b);

size_t damerauLevenshteinDistance(const std::string &a, const std::string &b);

size_t damerauLevenshteinDistance_beg(const std::string &a, size_t ind_beg, const std::string &b);

#endif

