#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include "Database.h"

using namespace std;

Database::Database(const string &db_filename){
    int rc = sqlite3_open(db_filename.c_str(), &(this->db));
    if (rc != SQLITE_OK) {
        sqlite3_close(this->db);
        throw BiblioException("Cannot open database: " + string(sqlite3_errmsg(this->db)));
    }
}

Database * Database::connect_database() {
    Config& cfg = Config::get_instance();
    string name = "database";
    try {
        if (cfg.exists(name) && cfg.lookup("database.enabled")){
            string filename = cfg.lookup("database.filename");
            Database * my_db = new Database(filename);    
            return my_db;
        } else {
            return nullptr;
        }
    } catch (BiblioException e) {
        return nullptr;
    }
}


int Database::check_status (const char * request, sqlite3_stmt **stmt) {
    int rc = sqlite3_prepare(db, request, -1, stmt, NULL);
    if(rc != SQLITE_OK) {
        cout << "status: " << rc << endl;
        throw BiblioException(sqlite3_errmsg(db));
    } else while((rc = sqlite3_step(*stmt)) != SQLITE_DONE) {
        switch(rc) {
            case SQLITE_BUSY:
                cout << "Please wait..." << endl;
                sleep(1);
                break;
            case SQLITE_ERROR:
                sqlite3_finalize(*stmt);
                throw BiblioException(sqlite3_errmsg(db));
                break;
            case SQLITE_ROW:
                return sqlite3_data_count(*stmt);
                break;
        }
    }
    return 0;
}

ArticleInfo * Database::get_data(std::string filename) {
    sqlite3_stmt *stmt;
    string request = "";

    request = "SELECT name FROM sqlite_master WHERE type='table'";

    int is_table = check_status(request.c_str(), &stmt);
    sqlite3_finalize(stmt);

    if (is_table == 0) {
        return nullptr;
    } else {

        request = "SELECT * FROM Data WHERE filename = \'" + filename + "\'";

        int is_paper = check_status(request.c_str(), &stmt);
        if (is_paper == 0) {
            sqlite3_finalize(stmt);
            return nullptr;
        }
        int id = sqlite3_column_int(stmt, 0);
        string lastmod_db = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11)));
        struct stat t_stat;
        stat(filename.c_str(), &t_stat);
        struct tm * timeinfo = localtime(&t_stat.st_mtim.tv_sec);
        string lastmod_file = asctime(timeinfo);

        if (lastmod_file <= lastmod_db){

            string title = unmark_quote(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))));
            string author_string = unmark_quote(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))));
            vector <string> authors = split(author_string, '|');
            string venue = unmark_quote(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))));
            string volume = unmark_quote(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5))));
            string number = unmark_quote(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6))));
            string pages = unmark_quote(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7))));
            string year = unmark_quote(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8))));
            string type = unmark_quote(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9))));
            string url = unmark_quote(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10))));
                      
           ArticleInfo * info = new ArticleInfo(title, authors, venue, volume,
                        number, pages, year, type, url);
            info->set_filename(filename);
            sqlite3_finalize(stmt);
            return info;
        } else {
            sqlite3_finalize(stmt);

            request = "DELETE FROM Data WHERE id = " + to_string(id);

            check_status(request.c_str(), &stmt);
            sqlite3_finalize(stmt);
            return nullptr;
        }
    }
}

void Database::add_data(const std::vector<ArticleInfo> &data) {
    size_t  data_size = data.size();
    if (data_size == 0) {
        return;
    }
    sqlite3_stmt *stmt = nullptr;
    string request = "";

    request = "SELECT name FROM sqlite_master WHERE type='table'";

    int is_table = check_status(request.c_str(), &stmt);
    sqlite3_finalize(stmt);

    if (is_table == 0) {
        request = "CREATE TABLE Data(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "filename TEXT, title TEXT, authors TEXT, venue TEXT, "
                "volume TEXT, number TEXT, pages TEXT, year TEXT, "
                "type TEXT, url TEXT, lastmod TEXT);";

        check_status(request.c_str(), &stmt);
        sqlite3_finalize(stmt);
    }

    struct stat t_stat;
    for (size_t i = 0; i < data_size; i++) {

        string filename = mark_quote(data[i].get_filename());
        string venue = mark_quote(data[i].get_venue());
        string volume = mark_quote(data[i].get_volume());
        string number = mark_quote(data[i].get_number());
        string pages = mark_quote(data[i].get_pages());
        string year = mark_quote(data[i].get_year());
        string type = mark_quote(data[i].get_type());
        string url = mark_quote(data[i].get_url());

        stat(filename.c_str(), &t_stat);
        struct tm *timeinfo = localtime(&t_stat.st_mtim.tv_sec);
        string lastmod_file = asctime(timeinfo);
        string title = mark_quote(data[i].get_title());
        vector<string> authors = data[i].get_authors();
        string author = "";
        if (authors.size() > 0) {
            for (size_t k = 0; k < authors.size() - 1; k++) {
                author += mark_quote(authors[k]) + "|";
            }
            author += mark_quote(authors[authors.size() - 1]);
        }
        request = "DELETE FROM DATA WHERE filename = \'" +  filename + "\';";

        check_status(request.c_str(), &stmt);
        sqlite3_finalize(stmt);

        request = "INSERT INTO Data(filename, title, authors, venue, "
                          "volume, number, pages, year, type, url, lastmod) VALUES ( \'" +
                  filename + "\', \'" + title + "\', \'" + author + "\', \'" + venue + "\', \'" +
                  volume + "\', \'" + number + "\', \'" + pages + "\', \'" + year + "\', \'" + type + "\', \'" +
                  url + "\', \'" + lastmod_file + "\');";
        check_status(request.c_str(), &stmt);
        sqlite3_finalize(stmt);
    }
}

inline bool exists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0); 
}

void Database::purge() {
    sqlite3_stmt *stmt;
    int rc;
    string request = "";

    request = "SELECT name FROM sqlite_master WHERE type='table'";
    int is_table = check_status(request.c_str(), &stmt);
    sqlite3_finalize(stmt);

    if (is_table == 0) {
        return;
    } else {
        vector <int> ids_to_purge = {};
        request = "SELECT * FROM Data ";
        rc = sqlite3_prepare(db, request.c_str(), -1, & stmt, NULL);
        if(rc != SQLITE_OK) {
            throw BiblioException(sqlite3_errmsg(db));
        } else while((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
            switch(rc) {
                case SQLITE_BUSY:
                    cout << "Please wait..." << endl;
                    sleep(1);
                    break;
                case SQLITE_ERROR:
                    sqlite3_finalize(stmt);
                    throw BiblioException(sqlite3_errmsg(db));
                    break;
                case SQLITE_ROW:
                    string paper_filename = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                    if (!exists(paper_filename)) {
                        ids_to_purge.push_back(sqlite3_column_int(stmt, 0));
                    }
            }
        }
        sqlite3_finalize(stmt);
        for (unsigned int i = 0; i < ids_to_purge.size(); i++) {
           request = "DELETE FROM DATA WHERE id = " + to_string(ids_to_purge[i]);
           check_status(request.c_str(), &stmt);
           sqlite3_finalize(stmt);
        }
    }   
}

Database::~Database(){
    sqlite3_close(db);
}
