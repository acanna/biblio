#include "ArticleInfo.h"

ArticleInfo::ArticleInfo(Json::Value info) {
	this->title = info["title"].asString();
	Json::Value authorsList = info["authors"].get("author", "");
	if (authorsList.size() > 0) {
		for (unsigned int i = 0; i < authorsList.size(); i++) {
			this->authors.push_back(authorsList[i].asString());
		}
	} else {
	this->authors.push_back(info["authors"].get("author", "").asString());
	}
	this->venue = info["venue"].asString();
	this->volume = info["volume"].asString();
	this->number = info["number"].asString();
	this->pages = info["pages"].asString();
	this->year = info["year"].asString();
	this->type = info["type"].asString();
	this->url = info["url"].asString();

} 

ArticleInfo::ArticleInfo(string title, vector <string> authors, string venue, string volume, 
string number, string pages, string year, string type, string url){
	this->title = title;
	this->authors = authors;
	this->venue = venue;
	this->volume = volume;
	this->number = number;
	this->pages = pages;
	this->year = year;
	this->type = type;
	this->url = url;
}

string ArticleInfo::to_String(){
	string output = "\n Title: \t" +this->title;
	output += "\n Authors: ";
	for (unsigned int i = 0; i < authors.size(); i++) {
		output += "\n\t\t" + this->authors[i];
	}
	output += "\n Venue: \t" + this->venue;
	output += "\n Volume: \t" + this->volume;
	output += "\n Number: \t" + this->number;
	output += "\n Pages: \t" + this->pages;
	output += "\n Year: \t\t" + this->year;
	output += "\n Type: \t\t" + this->type;
	output += "\n URL: \t\t" + this->url;
	return output;
}