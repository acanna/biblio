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

string const & ArticleInfo::get_title() const {
	return title;	
}

vector <string> const & ArticleInfo::get_authors() const {
	return authors;	
}
string const & ArticleInfo::get_venue() const {
	return venue;	
}
string const & ArticleInfo::get_volume() const {
	return volume;	
}
string const & ArticleInfo::get_number() const {
	return number;	
}
string const & ArticleInfo::get_pages() const {
	return pages;	
}
string const & ArticleInfo::get_year() const {
	return year;	
}
string const & ArticleInfo::get_type() const {
	return type;	
}
string const & ArticleInfo::get_url() const {
	return url;	
}
int ArticleInfo::get_precision() const {
    return precision;
}

void ArticleInfo::set_precision(int precision){
    this->precision = precision;
}


ArticleInfo::ArticleInfo(string const & title, vector <string> const & authors, 
string const & venue, string const & volume, string const & number, 
string const & pages, string const & year, string const & type, string const & url){
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
	string output = "\n Title: \t" + this->title;
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
	output += "\n Precision of response: \t" + std::to_string(this->precision) +"% \n";
	return output;
}
