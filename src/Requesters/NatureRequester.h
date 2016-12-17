#pragma once

#include "Requester.h"

class NatureRequester: public Requester {

protected:
    std::string url;

    std::vector<ArticleInfo> parse_response();
    std::string make_request(const std::string &query);

public:
    //EL: const&
    NatureRequester(std::string url);
};
