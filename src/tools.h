#ifndef TOOLS_H
#define TOOLS_H

#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> split(const std::string & str, char delimiter);

std::string low_letters_only(std::string str);

std::string delete_multiple_spaces(std::string str);

std::string short_name(std::string s);

std::string delete_junk_symbol(std::string str);

int levenshtein_distance(const std::string & s, const std::string & t);

#endif

