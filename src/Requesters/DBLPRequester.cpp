#include "DBLPRequester.h"

using namespace std;

DBLPRequester::DBLPRequester(string url):Requester() {
	this->url = url;
}

string DBLPRequester::make_request(const string &query){
	return this->url + query;
}

vector<ArticleInfo> DBLPRequester::parse_response(char * buffer) {
    Json::Reader reader;
    Json::Value root;

    bool parsingSuccessful = reader.parse(buffer, root);
    vector<ArticleInfo> articles;

    if (!parsingSuccessful) {
        string what = "DBLP: Parser error: " + reader.getFormattedErrorMessages();
        throw BiblioException(what);
    }

    Json::Value hits = root["result"]["hits"]["hit"];

    for (unsigned int i = 0; i < hits.size(); i++) {
        Json::Value info = hits[i]["info"];
	  	string title = info["title"].asString();

    	Json::Value authorsList = info["authors"].get("author", "");
	 	vector<string> authors = {};
		
		if (authorsList.size() > 0) {
	        for (unsigned int j = 0; j < authorsList.size(); j++) {
	            authors.push_back(authorsList[j].asString());
	        }
	    } else {
	        authors.push_back(info["authors"].get("author", "").asString());
	    }

	    string venue = "";
	    Json::Value venueList = info.get("venue", "");
	    if (venueList.size() > 0) {
	        for (unsigned int j = 0; j < venueList.size(); j++) {
	            venue += venueList[j].asString();
	            venue += " ";
	        }
	    } else {
	        venue = info.get("venue", "").asString();
	    }

	    string volume = info["volume"].asString();
	    string number = info["number"].asString();
	    string pages = info["pages"].asString();
	    string year = info["year"].asString();
	    string type = info["type"].asString();
	    string art_url = info["url"].asString();

        articles.push_back(ArticleInfo(title, authors, venue, volume, 
					number, pages, year, type, art_url));
    }  
	return articles;
}


