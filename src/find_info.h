#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include "Parser.h"
#include "DBLPManager.h"
#include "ArticleInfo.h"

std::vector <ArticleInfo> find_info(std::string filename, bool offline);
