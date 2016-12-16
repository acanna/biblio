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
    static void init(const std::string &filename);
    bool exists(const std::string &s) const;
    libconfig::Setting& lookup(const std::string & s) const;
    bool lookupValue(const std::string & s, int & res) const;
    bool lookupValue(const std::string &  s, std::string & res) const;
};
