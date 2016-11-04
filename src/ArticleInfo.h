#pragma once

#include <vector>
#include <cstring>
#include <string>
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
    int precision;

public:
	ArticleInfo (Json::Value const & info) ;

	std::string const & get_title();
	std::vector <std::string> const & get_authors();
	std::string const & get_venue();
	std::string const & get_volume();
	std::string const & get_number();
	std::string const & get_pages();
	std::string const & get_year();
	std::string const & get_type();
	std::string const & get_url();
	int get_precision() const; 
    void set_precision(int precision);

	ArticleInfo (std::string const & title, std::vector <std::string> const & authors, 
		     std::string const & venue, std::string const & volume, std::string const & number,
		     std::string const & pages, std::string const & year, std::string const & type, 
		     std::string const & url);
	
	std::string const & to_string();
};

