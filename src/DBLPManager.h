#pragma once

#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <fstream> 
#include <vector>
#include <exception>
#include <stdexcept>
#include "../lib/json/json.h"
#include "../lib/json/value.h"
#include "ArticleInfo.h"


#define MAX_BUF 65536

const std::string URL = "http://dblp.org/search/publ/api?format=json&h=1&q=";

class DBLPManager {

private:
    int errorCode;
    CURL *curl;

private:
    std::vector<ArticleInfo> parseResponse();

public:
    DBLPManager();
    virtual ~DBLPManager();
    std::vector<ArticleInfo> publicationRequest(const std::string &query);
};
