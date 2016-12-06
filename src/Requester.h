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
#include "Biblio_exception.h"

#define MAX_BUF 65536



class Requester {

protected:
	CURL * curl;
    char buffer[MAX_BUF + 1];
    int bufferIndex;
    int errorCode;

protected:
	static size_t writeData(void *webBuffer, size_t size, size_t nmemb, void *userp);
	void curl_init();
	void curl_clean_up();
	char * curl_perform(std::string request) ;
	virtual std::vector<ArticleInfo> parse_response(char* buffer) = 0;
	virtual std::string make_request(std::string query) = 0;

public:
	Requester();
    Requester(const Requester& r);
    virtual ~Requester();
    virtual std::vector<ArticleInfo> publication_request(const std::string &query);

};

