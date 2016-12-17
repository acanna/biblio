#include <regex>

#include "NatureRequester.h"

using namespace std;

NatureRequester::NatureRequester(const string& url):Requester() {
    this->url = url;
}

string NatureRequester::make_request(const string &query){
    string new_query = regex_replace(query, regex(" "), "+");
    string s = this->url + "dc.title+all+" +new_query + "&queryType=cql&httpAccept=application/json&sortKeys=,pam,0&recordPacking=unpacked&maximumRecords=5";
    return s;
}

std::vector<ArticleInfo> NatureRequester::parse_response() {
    Json::Reader reader;
    Json::Value root;

    bool parsingSuccessful = reader.parse(buffer, root);

    vector<ArticleInfo> articles;

    if (!parsingSuccessful) {
        string what = "Nature: Parser error: " + reader.getFormattedErrorMessages();
        cout << what << endl;
		return articles;
    }

    Json::Value entry = root["feed"]["entry"];

    for (unsigned int i = 0; i < entry.size(); i++) {
        string title = entry[i]["title"].asString();

        Json::Value authorsList = entry[i].get("dc:creator", "");
        vector<string> authors = {};

        if (authorsList.size() > 0) {
            for (unsigned int j = 0; j < authorsList.size(); j++) {
                authors.push_back(authorsList[j].asString());
            }
        } else {
            authors.push_back(entry[i]["dc:creator"].get("author", "").asString());
        }

        string venue = entry[i].get("prism:publicationName", "").asString();

        string volume = entry[i]["prism:volume"].asString();
        string number = entry[i]["prism:number"].asString();

        string pages = "";
        string start_page = entry[i]["prism:startingPage"].asString();

        if (start_page == ""){
            pages = entry[i]["prism:doi"].asString();
        } else {
            string end_page = entry[i]["prism:endingPage"].asString();
            if (start_page == end_page) {
                pages = start_page;
            } else {
                pages = start_page + "-" + end_page;
            }
        }

        string year = entry[i]["prism:publicationDate"].asString();
        year = year.substr(0, 4);

        string type = entry[i]["prism:genre"].asString();
        string art_url = entry[i]["prism:url"].asString();


        articles.push_back(ArticleInfo(title, authors, venue, volume,
                    number, pages, year, type, art_url));
    }  
    return articles;
}


