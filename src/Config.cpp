#include <iostream> 
#include "Config.h"

using namespace std;

string Config::filename;

Config::Config(){
    try {
        cfg.readFile(filename.c_str());
    } catch (const libconfig::FileIOException &ex) {
        throw BiblioException("Reading config file failed");
    } catch (const libconfig::ParseException &pex) {
        string what = "Parse error at " + (string) pex.getFile() + ":"
                      + to_string(pex.getLine()) + " - " + (string) pex.getError();
        throw BiblioException("Reading config file failed: " + what);
    }
}

void Config::init(const string &s){
	filename = s;
}

bool Config::exists(const string &s) const {
	return cfg.exists(s);
}

libconfig::Setting& Config::lookup(const string & s) const {
	if (!cfg.exists(s)) {
		throw BiblioException("Wrong config file - missing " + s);
	}
	return cfg.lookup(s);
}

bool Config::lookupValue(const string & s, int & res) const {
	return cfg.lookupValue(s, res);
}

bool Config::lookupValue(const string & s, string & res) const {
	return cfg.lookupValue(s, res);
}

 Config & Config::get_instance() {
	static Config config;
	return config;
}
