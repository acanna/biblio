#pragma once

#include <libconfig.h++>

#include "Requesters/Requester.h"

//EL: мне кажется, это функции не меняют состояние класса и им нужен const
class Config { // singleton
private:
	libconfig::Config cfg;
	static std::string filename;
 
	Config ();
	Config(Config const &);
	Config& operator= (Config const &);

public: 
	static Config & get_instance (); 
	//EL: const &
	static void init(std::string filename);
	//EL: const &
	bool exists(std::string s);
	//EL: const &
	libconfig::Setting& lookup(std::string s);
	//EL: const &
	bool lookupValue(std::string s, int & res);
	//EL: const &
	bool lookupValue(std::string s, std::string & res);
};
