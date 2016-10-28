#include "ArticleInfo.h"
using std::vector;
using std::string;

ArticleInfo::ArticleInfo(Json::Value const & info) {
	this->title = info["title"].asString();
	Json::Value authorsList = info["authors"].get("author", "");
	if (authorsList.size() > 0) {
		for (unsigned int i = 0; i < authorsList.size(); i++) {
			this->authors.push_back(authorsList[i].asString());
		}
	} else {
	this->authors.push_back(info["authors"].get("author", "").asString());
	}
	string venue ="";
	Json::Value venueList = info.get("venue", "");
	if (venueList.size() > 0) {
		for (unsigned int i = 0; i < venueList.size(); i++) {
			venue += venueList[i].asString();
			venue += " ";
		}
	} else {
		venue = info.get("venue", "").asString();
	}
	this->venue = venue;
	this->volume = info["volume"].asString();
	this->number = info["number"].asString();
	this->pages = info["pages"].asString();
	this->year = info["year"].asString();
	this->type = info["type"].asString();
	this->url = info["url"].asString();
    this->precision = 0;
} 

string ArticleInfo::get_title(){
	return title;	
}

vector <string> ArticleInfo::get_authors(){
	return authors;	
}
string ArticleInfo::get_venue(){
	return venue;	
}
string ArticleInfo::get_volume(){
	return volume;	
}
string ArticleInfo::get_number(){
	return number;	
}
string ArticleInfo::get_pages(){
	return pages;	
}
string ArticleInfo::get_year(){
	return year;	
}
string ArticleInfo::get_type(){
	return type;	
}
string ArticleInfo::get_url(){
	return url;	
}
int ArticleInfo::get_precision() const{
    return precision;
}

void ArticleInfo::set_precision(int precision){
    this->precision = precision;
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
    this->precision = 0;
}

string ArticleInfo::to_string(){
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
	output += "\n precision of request: \t" + std::to_string(this->precision) +"% \n\n";
	return output;
}
