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

void Config::init(string s){
	filename = s;
}

bool Config::exists(string s){
	return cfg.exists(s);
}

libconfig::Setting& Config::lookup(string s) {
	if (!cfg.exists(s)) {
		throw BiblioException("Wrong config file - missing " + s);
	}
	return cfg.lookup(s);
}

bool Config::lookupValue(string s, int & res) {
	return cfg.lookupValue(s, res);
}

bool Config::lookupValue(string s, string & res) {
	return cfg.lookupValue(s, res);
}

 Config & Config::get_instance() {
	static Config config;
	return config;
}
