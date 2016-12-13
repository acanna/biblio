#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>

#include "BiblioException.h"
#include "ArticleInfo.h"
#include "Config.h"
#include "tools.h"

//EL не уверен, что все эти функции должны быть публичными
//EL у класса должны быть поля (например, sqlite3 *db;)
//EL static скорей всего быть не должно
class Database {

private:
    std::string db_filename;

public:
    Database(const std::string &db_filename);
	static Database * connect_database();
	static int check_status (const char * request, sqlite3 *db, int rc, sqlite3_stmt **stmt);
    ArticleInfo * get_data(std::string filename);
    //const & (особенно у info)
    void add_data(std::string filename, ArticleInfo info);
    void add_data(std::vector<ArticleInfo> &data);
    static void purge();
};

