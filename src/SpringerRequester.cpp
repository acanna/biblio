#include "SpringerRequester.h"
#include <regex>
#include <string>

using namespace std;

const std::string URL = "http://api.springer.com/meta/v1/json?q=title:\"";
const std::string APIKEY = "\"&api_key=64f779d62e09f8ec669d4c656684cded";

string SpringerRequester::make_request(std::string query){
    regex re_frmt(" ");
    query = regex_replace(query, re_frmt, "%20");
	return URL + query + APIKEY;
}

vector<ArticleInfo> SpringerRequester::parse_response(char * buffer)  {
    Json::Reader reader;
    Json::Value root;

    bool parsingSuccessful = reader.parse(buffer, root);
    vector<ArticleInfo> articles;

    if (!parsingSuccessful) {
        string what = "Springer: Parser error: " + reader.getFormattedErrorMessages();
        throw Biblio_exception(what);
    }

	int size = stoi(root["result"][0].get("total", "").asString());
		
	Json::Value records = root["records"];

    for (int i = 0; i < size; i++) {
        Json::Value info = records[i];
	    string title = info["title"].asString();
		
	    Json::Value authorsList = info["creators"];
		vector<string> authors = {};	    

		if (authorsList.size() > 0) {
	        for (unsigned int i = 0; i < authorsList.size(); i++) {
	            authors.push_back(authorsList[i].get("creator", "").asString());
	        }
	    } else {
	        authors.push_back(info["creators"].get("creator", "").asString());
	    }

	    string venue = info.get("publicationName", "").asString();
	    string volume = info.get("volume", "").asString();
	    string number = info.get("number", "").asString();
	    string start_page = info.get("startingPage","").asString();
		string end_page = info.get("endingPage","").asString();
		string pages = start_page +"-" + end_page;
		string year = info.get("publicationDate", "").asString();
	    year = year.substr(0,4);
	    string type = info.get("issuetype","").asString();
	    string art_url = info["url"][0].get("value","").asString();
	
        articles.push_back(ArticleInfo(title, authors, venue, volume, 
					number, pages, year, type, art_url));
    }

    return articles;
}


