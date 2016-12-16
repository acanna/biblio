#pragma once

#include "Requester.h"

class ScienceDirectRequester: public Requester {

protected:
    std::string url;
    std::string apikey;

    std::vector<ArticleInfo> parse_response(char * buffer);
    std::string make_request(const std::string &query);

public:
    ScienceDirectRequester(const std::string &url, const std::string &apikey);
};
