#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include "Parser.h"
#include "DBLPManager.h"
#include "ArticleInfo.h"
#include "Biblio_exception.h"


std::vector <ArticleInfo> find_info(const std::string & filename, bool offline);
void print_info( std::vector <ArticleInfo> & result);
void printf_info(const std::string & filename, std::vector <ArticleInfo> & result);