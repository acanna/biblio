#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>

#include "BiblioException.h"
#include "ArticleInfo.h"
#include "Config.h"
#include "tools.h"

class Database {

private:
    std::string db_filename;
    sqlite3 * db;

public:
    Database(const std::string &db_filename);
    ~Database();
    static Database * connect_database();
    int check_status (const char * request, sqlite3_stmt **stmt);
    ArticleInfo * get_data(std::string filename);
    void add_data(const std::vector<ArticleInfo> &data);
    void purge();
};

