#pragma once

#include <iostream> 
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include "Parser.h"
#include "DBLPManager.h"
#include "ArticleInfo.h"
#include "Biblio_exception.h"


std::vector <ArticleInfo> find_info(const std::string & filename, bool offline);

void print_html(std::ostream & out, const std::string & filename, std::vector <ArticleInfo> & result);
void print_bib(std::ostream & out, const std::string & filename, std::vector <ArticleInfo> & result);
void print_txt(std::ostream & out, const std::string & filename, std::vector <ArticleInfo> & result);

bool greater (const ArticleInfo & info_1, const ArticleInfo & info_2);

std::vector <ArticleInfo> search_dblp (DBLPManager & dblp, std::string & query);

int levenshtein_distance(const std::string & s, const std::string & t);


