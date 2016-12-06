#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "Database.h"

using namespace std;

Database::Database(std::string db_filename){
    this->db_filename = db_filename;
}

int check_status (const char * request, sqlite3 *db, int rc, sqlite3_stmt **stmt) {
	rc = sqlite3_prepare(db, request, -1, stmt, NULL);
	if(rc != SQLITE_OK) {
		cout << "status: " << rc << endl;
		sqlite3_close(db);
		throw Biblio_exception(sqlite3_errmsg(db));
	} else while((rc = sqlite3_step(*stmt)) != SQLITE_DONE) {
		switch(rc) {
    		case SQLITE_BUSY: 
    			cout << "Please wait..." << endl;
    			sleep(1);
    			break;	
    		case SQLITE_ERROR:
				sqlite3_finalize(*stmt);
    			sqlite3_close(db);
		        throw Biblio_exception(sqlite3_errmsg(db));
				break;
    		case SQLITE_ROW:
                return sqlite3_data_count(*stmt);
				break;
		}
	}
	return 0;
}

ArticleInfo * Database::get_data(std::string filename) {
    sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc;
	string request = "";

	rc = sqlite3_open(this->db_filename.c_str(), &db);
	if (rc != SQLITE_OK) {
		sqlite3_close(db);
		throw Biblio_exception("Cannot open database " + string(sqlite3_errmsg(db)));
	}

	request = "SELECT name FROM sqlite_master WHERE type='table'";
	int is_table = check_status(request.c_str(), db, rc, &stmt);
	sqlite3_finalize(stmt);

    if (is_table == 0) {
		sqlite3_close(db);
		return nullptr;
    } else {
	    request = "SELECT * FROM Data WHERE filename = \'" + filename + "\'";
		int is_paper = check_status(request.c_str(), db, rc, &stmt);
		if (is_paper == 0) {
			sqlite3_finalize(stmt);
			sqlite3_close(db);	
			return nullptr;
		}
		int id = sqlite3_column_int(stmt, 0);
	    string lastmod_db = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11)));
	    struct stat t_stat;
	    stat(filename.c_str(), &t_stat);
	    struct tm * timeinfo = localtime(&t_stat.st_mtim.tv_sec);
	    string lastmod_file = asctime(timeinfo);

	    if (lastmod_file <= lastmod_db){

		    string title = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))); 
		    string author_string = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
		    vector <string> authors = {};
		    authors.push_back(author_string); 
		    string venue = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
		    string volume = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
		    string number = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
		    string pages = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
		    string year = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
		    string type = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)));
		    string url = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10)));
		    ArticleInfo * info = new ArticleInfo(title, authors, venue, volume, 
						number, pages, year, type, url);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
		    return info;
		} else {
			sqlite3_finalize(stmt);	

			request = "DELETE FROM Data WHERE id = " + to_string(id);
			check_status(request.c_str(), db, rc, &stmt);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return nullptr;
	    }
	}

}

void Database::add_data(string filename, ArticleInfo info) {
    sqlite3 *db;
	sqlite3_stmt *stmt = nullptr;
	int rc;
	string request = "";


	rc = sqlite3_open(this->db_filename.c_str(), &db);
	if (rc) {
		sqlite3_close(db);
		throw Biblio_exception("Cannot open database " + string(sqlite3_errmsg(db)));
	}

	request = "SELECT name FROM sqlite_master WHERE type='table'";
	int is_table = check_status(request.c_str(), db, rc, &stmt);
	sqlite3_finalize(stmt);

    if (is_table == 0) {
        request = "CREATE TABLE Data(id INTEGER PRIMARY KEY AUTOINCREMENT, "
	    "filename TEXT, title TEXT, authors TEXT, venue TEXT, "
    	"volume TEXT, number TEXT, pages TEXT, year TEXT, "
    	"type TEXT, url TEXT, lastmod TEXT);";
	
		check_status(request.c_str(), db, rc, &stmt);
		sqlite3_finalize(stmt);
	} 
    
    struct stat t_stat;
    stat(filename.c_str(), &t_stat);
    struct tm * timeinfo = localtime(&t_stat.st_mtim.tv_sec);
    string lastmod_file = asctime(timeinfo);
    string title = info.get_title();
	vector<string> authors = info.get_authors();
   	string author = "";
	for (unsigned int i = 0; i < authors.size(); i++){
    	author += authors[i] + " ";
	}

    string venue = info.get_venue();
    string volume = info.get_volume();
    string number = info.get_number();
    string pages = info.get_pages();
    string year = info.get_year();
    string type = info.get_type();
    string url  = info.get_url();

	request = "INSERT INTO Data(filename, title, authors, venue, "
		"volume, number, pages, year, type, url, lastmod) VALUES ( \'" +
		filename + "\', \'" + title + "\', \'" + author + "\', \'" + venue+ "\', \'" + 
		volume + "\', \'" + number + "\', \'" + pages + "\', \'" + year + "\', \'" + type + "\', \'" +
		url+ "\', \'" +  lastmod_file + "\');";


	check_status(request.c_str(), db, rc, &stmt);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
}


