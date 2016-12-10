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

public:
    Database(const std::string &db_filename);
	static Database * connect_database();
	static int check_status (const char * request, sqlite3 *db, int rc, sqlite3_stmt **stmt);
    ArticleInfo * get_data(std::string filename);
    void add_data(std::string filename, ArticleInfo info);
    void add_data(std::vector<ArticleInfo> &data);
    static void purge();
};

