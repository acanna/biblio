#pragma once
#include "Requester.h"

class DBLPRequester: public Requester {

protected:
    std::vector<ArticleInfo> parse_response(char * buffer);
	std::string make_request(std::string query);
};
