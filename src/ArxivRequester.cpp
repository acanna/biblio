#include "ArxivRequester.h"

using namespace std;

ArxivRequester::ArxivRequester(string url):Requester() {
	this->url = url;
}

string ArxivRequester::make_request(std::string query){
	return this->url + query;
}

vector<ArticleInfo> ArxivRequester::parse_response(char * buffer) {
    vector<ArticleInfo> articles = {};
    
	return articles;
}


