#pragma once

#include "Requester.h"

class ScopusRequester: public Requester {

protected:
    std::string url;
    std::string apikey;

    std::vector<ArticleInfo> parse_response(char * buffer);
    std::string make_request(const std::string &query);

public:
    ScopusRequester(const std::string &url, const std::string &apikey);
};
