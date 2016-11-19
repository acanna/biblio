#pragma once
#include "Requester.h"

class SpringerRequester: public Requester {

protected:
    std::vector<ArticleInfo> parse_response(char * buffer);
	std::string make_request(std::string query);
};
