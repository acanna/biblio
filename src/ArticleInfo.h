#pragma once

#include <vector>
#include <cstring>
#include "json/json.h"
#include "json/value.h" 

class ArticleInfo{

	std::string title;
	std::vector <std::string> authors;
	std::string venue;
	std::string volume;
	std::string number;
	std::string pages;
	std::string year;
	std::string type;
	std::string url;

public:
	ArticleInfo (Json::Value const & info) ;

	std::string get_title();
	std::vector <std::string> get_authors();		
	std::string get_venue();
	std::string get_volume();
	std::string get_number();
	std::string get_pages();
	std::string get_year();
	std::string get_type();
	std::string get_url();


	ArticleInfo (const std::string title, std::vector <std::string> authors, 
		     const std::string venue, const std::string volume, const std::string number,
		     const std::string pages, const std::string year, const std::string type, 
		     const std::string url);
	
	std::string to_string();
};