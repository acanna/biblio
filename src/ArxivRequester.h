#pragma once
#include "Requester.h"
#include "Biblio_exception.h"
#include <tinyxml2.h>



class ArxivRequester: public Requester {

protected:
	std::string url;
	std::string apikey;  

    std::vector<ArticleInfo> parse_response(char * buffer);
	std::string make_request(std::string query);

public:
	ArxivRequester(std::string url);
};
