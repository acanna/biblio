#pragma once

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream> 
#include <vector>
#include <exception>
#include <stdexcept>

#include <curl/curl.h>
#include "../../lib/json/json.h"
#include "../../lib/json/value.h"

#include "../ArticleInfo.h"
#include "../BiblioException.h"

#define MAX_BUF 65536

class Requester {

protected:
    CURL * curl;
    char buffer[MAX_BUF + 1];
    int bufferIndex;
    int errorCode;

protected:
    static size_t writeData(void *webBuffer, const size_t size, const size_t nmemb, void *userp);
    void curl_init();
    void curl_clean_up();
    void curl_perform(const std::string &request) ;
    virtual std::vector<ArticleInfo> parse_response() = 0;
    virtual std::string make_request(const std::string &query) = 0;

public:
    Requester();
    Requester(const Requester& r);
    virtual ~Requester();
    virtual std::vector<ArticleInfo> publication_request(const std::string &query);

};

