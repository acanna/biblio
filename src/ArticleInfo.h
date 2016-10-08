#pragma once

#include <vector>
#include <cstring>
#include "json/json.h"
#include "json/value.h" 
using std::vector;
using std::string;


class ArticleInfo{
	string title;
	vector <string> authors;
	string venue;
	string volume;
	string number;
	string pages;
	string year;
	string type;
	string url;

public:
	ArticleInfo (Json::Value info) ;

	ArticleInfo (string title, vector <string> authors, string venue, string volume, string number, string pages, string year, string type, string url);
	
	string to_String();
};