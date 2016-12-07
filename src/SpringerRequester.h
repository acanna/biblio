#pragma once

#include "Requester.h"

class SpringerRequester: public Requester {

protected:
	std::string url;
	std::string apikey;  

    std::vector<ArticleInfo> parse_response(char * buffer);
	std::string make_request(const std::string &query);

public:
	SpringerRequester(const std::string &url, const std::string &apikey);
};
