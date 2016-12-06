#pragma once

#include <vector>
#include <string>
#include <sstream>
#include "Biblio_exception.h"
#include "Requester.h"
#include "DBLPRequester.h"
#include "SpringerRequester.h"
#include "ArxivRequester.h"
#include "NatureRequester.h"
#include "ScopusRequester.h"
#include "ScienceDirectRequester.h"

enum requestersEnum {
    dblp = 1,
    springer = 2,
    arxiv = 3,
    nature = 4,
    sciencedirect = 5,
    scopus = 6
};

std::vector<Requester *> init_requesters(std::vector<std::pair<requestersEnum, std::vector<std::string>>> & data);
std::vector<Requester *> read_config(const std::string &filename, int &threads);
std::vector<std::pair<requestersEnum, std::vector<std::string>>> read_config_data(const std::string &filename, int &threads);
std::vector<std::string> split(const std::string &str, char delimiter);
std::string low_letters_only(std::string str);
std::string delete_spaces_to_lower(std::string str);

std::string short_name(std::string s);
std::string raw_to_formatted(std::string s);
std::string letters_to_lower(std::string s);
size_t levenshtein_distance(const std::string &s, const std::string &t);
std::vector<std::string> read_pdf_files_recursive(std::string &path);
void read_pdf_files(std::vector<std::string>& v, std::string& path);

