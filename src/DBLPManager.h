#pragma once

#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <fstream> 
#include <vector>
#include <exception>
#include <stdexcept>
#include "json/json.h"
#include "json/value.h"
#include "ArticleInfo.h"


#define MAX_BUF 65536

const std::string URL = "http://dblp.org/search/publ/api?format=json&q=";

class DBLPManager {

	int errorCode;
	CURL *curl;

public:

	DBLPManager();

	virtual ~DBLPManager();

	std::vector <ArticleInfo> publicationRequest(const std::string & query);

private:

	std::vector <ArticleInfo> parseResponse();
};
