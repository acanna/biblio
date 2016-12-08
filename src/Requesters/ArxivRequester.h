#pragma once

#include "Requester.h"
#include <tinyxml2.h>

class ArxivRequester: public Requester {

protected:
	std::string url;

    std::vector<ArticleInfo> parse_response(char * buffer);
	std::string make_request(const std::string &query);

public:
	ArxivRequester(const std::string &url);
};
