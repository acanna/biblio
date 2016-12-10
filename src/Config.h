#pragma once

#include <libconfig.h++>

#include "Requesters/Requester.h"

class Config { // singleton
private:
	libconfig::Config cfg;
	static std::string filename;
 
	Config ();
	Config(Config const &);
	Config& operator= (Config const &);

public: 
	static Config & get_instance (); 
	static void init(std::string filename);
	bool exists(std::string s);
	libconfig::Setting& lookup(std::string s);
	bool lookupValue(std::string s, int & res);
	bool lookupValue(std::string s, std::string & res);
};
