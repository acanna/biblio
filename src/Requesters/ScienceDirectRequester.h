#pragma once

#include "Requester.h"

class ScienceDirectRequester: public Requester {

protected:
    std::string url;
    std::string apikey;
    struct curl_slist *chunk;

    std::vector<ArticleInfo> parse_response();
    std::string make_request(const std::string &query);

public:
    ScienceDirectRequester(const std::string &url, const std::string &apikey);
    ~ScienceDirectRequester() { if (chunk != NULL)  curl_slist_free_all(chunk);};
};
