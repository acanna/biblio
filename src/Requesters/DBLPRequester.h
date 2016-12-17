#pragma once

#include "Requester.h"

class DBLPRequester: public Requester {

protected:
    std::string url;

    std::vector<ArticleInfo> parse_response();
    std::string make_request(const std::string &query);

public:
    DBLPRequester(const std::string &url);
};
