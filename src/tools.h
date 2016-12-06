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
#include "Database.h"

std::vector<Requester *> read_config(const std::string &filename);
//EL хорошо бы убрать внутрь Database
Database * connect_database(const std::string &filename);

std::vector<std::string> split(const std::string &str, char delimiter);

//const & у string
std::string low_letters_only(std::string str);
std::string delete_spaces_to_lower(std::string str);

std::string short_name(std::string s);
std::string raw_to_formatted(std::string s);

std::string letters_to_lower(std::string s);
size_t levenshtein_distance(const std::string &s, const std::string &t);
std::vector<std::string> read_pdf_files_recursive(std::string &path);
void read_pdf_files(std::vector<std::string>& v, std::string& path);

