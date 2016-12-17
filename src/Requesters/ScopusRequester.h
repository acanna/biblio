#pragma once

#include "Requester.h"

class ScopusRequester: public Requester {

protected:
    std::string url;
    std::string apikey;
    struct curl_slist *chunk;

    std::vector<ArticleInfo> parse_response();
    std::string make_request(const std::string &query);

public:
    ScopusRequester(const std::string &url, const std::string &apikey);
    ~ScopusRequester() { if (chunk != NULL)  curl_slist_free_all(chunk);};
};
