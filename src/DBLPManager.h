#pragma once

#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <fstream> 
#include <vector>
#include "json/json.h"
#include "json/value.h"
#include "ArticleInfo.h"

using std::string;
using std::vector;

#define MAX_BUF 65536
const string URL = "http://dblp.org/search/publ/api?format=json&q=";

class DBLPManager {
	//char buffer[MAX_BUF + 1];
	//int bufferIndex;
	int errorCode;
	CURL *curl;

	public:	
	DBLPManager();
	virtual ~DBLPManager();
	vector <ArticleInfo> publicationRequest(string query);

	private:
	vector <ArticleInfo> parseResponse();
	//size_t writeData (void * webBuffer, size_t size, size_t nmemb, void *userp); 
};
