#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "Biblio_exception.h"
#include "ArticleInfo.h"

class Database {

private:
    std::string db_filename;

public:
    //const string &
    Database(std::string db_filename);
    ArticleInfo * get_data(std::string filename);
    void add_data(std::string filename, ArticleInfo info);
};

