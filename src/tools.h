#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "BiblioException.h"
#include "Requesters/DBLPRequester.h"
#include "Requesters/SpringerRequester.h"
#include "Requesters/ArxivRequester.h"
#include "Requesters/NatureRequester.h"
#include "Requesters/ScopusRequester.h"
#include "Requesters/ScienceDirectRequester.h"
#include "Database.h"

std::vector<std::string> split(const std::string &str, char delimiter);
std::string get_exe_path();
std::string low_letters_only(std::string str);
std::string delete_spaces_to_lower(std::string str);

std::string short_name(const std::string s);
std::string raw_to_formatted(const std::string s);

std::string letters_to_lower(const std::string s);
size_t levenshtein_distance(const std::string &s, const std::string &t);
std::vector<std::string> read_pdf_files_recursive(std::string &path);
void read_pdf_files(std::vector<std::string>& v, std::string& path);

