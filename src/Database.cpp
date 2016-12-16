#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include "Database.h"

using namespace std;

//EL: постоянно делается open/close db, это не тру!
//EL: один раз в начале программы октрыли (например, в конструкторе)
//EL: в конце закрыли (например, в деструкторе)
Database::Database(const string &db_filename){
    this->db_filename = db_filename;
}

Database * Database::connect_database() {
    //EL: загадочно! не похоже, чтобы этот метод открывал db
    Config& cfg = Config::get_instance();
    string name = "database";
    Database * db;
    if (cfg.exists(name) && cfg.lookup("database.enabled")){
        string filename = cfg.lookup("database.filename");
        db = new Database(filename);
        return db;
    } else {
        return nullptr;
    }
}

//EL: db должно быть полем
int Database::check_status (const char * request, sqlite3 *db, int rc, sqlite3_stmt **stmt) {
    rc = sqlite3_prepare(db, request, -1, stmt, NULL);
    if(rc != SQLITE_OK) {
        cout << "status: " << rc << endl;
        sqlite3_close(db);
        throw BiblioException(sqlite3_errmsg(db));
    } else while((rc = sqlite3_step(*stmt)) != SQLITE_DONE) {
        switch(rc) {
            case SQLITE_BUSY:
                cout << "Please wait..." << endl;
                sleep(1);
                break;
            case SQLITE_ERROR:
                sqlite3_finalize(*stmt);
                sqlite3_close(db);
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
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;
    string request = "";
    //EL: зачем каждый раз открывать базу?
    rc = sqlite3_open(this->db_filename.c_str(), &db);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        throw BiblioException("Cannot open database " + string(sqlite3_errmsg(db)));
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
            vector <string> authors = split(author_string, '|');
            string venue = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            string volume = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
            string number = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
            string pages = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
            string year = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
            string type = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)));
            string url = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10)));

            ArticleInfo * info = new ArticleInfo(title, authors, venue, volume,
                        number, pages, year, type, url);
            //EL: не нашел места в программе, где есть парный delete к этому new
            info->set_filename(filename);
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
        throw BiblioException("Cannot open database " + string(sqlite3_errmsg(db)));
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
    
    request = "DELETE FROM DATA WHERE filename = \'" +  filename + "\';";

    check_status(request.c_str(), db, rc, &stmt);
    sqlite3_finalize(stmt);

    struct stat t_stat;
    stat(filename.c_str(), &t_stat);
    struct tm * timeinfo = localtime(&t_stat.st_mtim.tv_sec);
    string lastmod_file = asctime(timeinfo);
    string title = info.get_title();
    vector<string> authors = info.get_authors();

    string author = "";
    if (authors.size() > 0) {
        for (unsigned int i = 0; i < authors.size()-1; i++){
            author += authors[i] + "|";
        }
        author += authors[authors.size() -1];
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

//EL: если сделать нормальное открытие/закрытие db и реализовать этот метод
//не через copy-paste, а вызовом в цикле Database::add_data(string filename, ArticleInfo info) 
//то будет короче и лучше
void Database::add_data(std::vector<ArticleInfo> &data) {
    size_t  data_size = data.size();
    if (data_size == 0) {
        return;
    }
    sqlite3 *db;
    sqlite3_stmt *stmt = nullptr;
    int rc;
    string request = "";


    rc = sqlite3_open(this->db_filename.c_str(), &db);
    if (rc) {
        sqlite3_close(db);
        throw BiblioException("Cannot open database " + string(sqlite3_errmsg(db)));
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
    for (size_t i = 0; i < data_size; i++) {

        string filename = data[i].get_filename();
        string venue = data[i].get_venue();
        string volume = data[i].get_volume();
        string number = data[i].get_number();
        string pages = data[i].get_pages();
        string year = data[i].get_year();
        string type = data[i].get_type();
        string url = data[i].get_url();

        stat(filename.c_str(), &t_stat);
        struct tm *timeinfo = localtime(&t_stat.st_mtim.tv_sec);
        string lastmod_file = asctime(timeinfo);
        string title = data[i].get_title();
        vector<string> authors = data[i].get_authors();
        string author = "";
        if (authors.size() > 0) {
            for (size_t k = 0; k < authors.size() - 1; k++) {
                author += authors[k] + "|";
            }
            author += authors[authors.size() - 1];
        }
        request = "DELETE FROM DATA WHERE filename = \'" +  filename + "\';";

        check_status(request.c_str(), db, rc, &stmt);
        sqlite3_finalize(stmt);

        request = "INSERT INTO Data(filename, title, authors, venue, "
                          "volume, number, pages, year, type, url, lastmod) VALUES ( \'" +
                  filename + "\', \'" + title + "\', \'" + author + "\', \'" + venue + "\', \'" +
                  volume + "\', \'" + number + "\', \'" + pages + "\', \'" + year + "\', \'" + type + "\', \'" +
                  url + "\', \'" + lastmod_file + "\');";

        check_status(request.c_str(), db, rc, &stmt);
        sqlite3_finalize(stmt);
    }
    sqlite3_close(db);
}

inline bool exists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0); 
}

void Database::purge() {
    Config& cfg = Config::get_instance();
    string name = "database";
    string filename;
    if (!cfg.exists(name) || !cfg.lookupValue("database.filename", filename)) {
        throw BiblioException ("No database found in config file");
    }
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;
    string request = "";

    rc = sqlite3_open(filename.c_str(), &db);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        throw BiblioException("Cannot open database " + string(sqlite3_errmsg(db)));
    }

    request = "SELECT name FROM sqlite_master WHERE type='table'";
    int is_table = check_status(request.c_str(), db, rc, &stmt);
    sqlite3_finalize(stmt);

    if (is_table == 0) {
        sqlite3_close(db);
        return;
    } else {
        vector <int> ids_to_purche = {};
        request = "SELECT * FROM Data ";
        rc = sqlite3_prepare(db, request.c_str(), -1, & stmt, NULL);
        if(rc != SQLITE_OK) {
            cout << "status: " << rc << endl;
            sqlite3_close(db);
            throw BiblioException(sqlite3_errmsg(db));
        } else while((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
            switch(rc) {
                case SQLITE_BUSY:
                    cout << "Please wait..." << endl;
                    sleep(1);
                    break;
                case SQLITE_ERROR:
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    throw BiblioException(sqlite3_errmsg(db));
                    break;
                case SQLITE_ROW:
                    string paper_filename = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                    cout << "Next paper: " <<  sqlite3_column_int(stmt, 0) << " "  << paper_filename << endl;
                    if (!exists(paper_filename)) {
                        //EL: purche?
                        ids_to_purche.push_back(sqlite3_column_int(stmt, 0));
                    }
            }
        }
        sqlite3_finalize(stmt);
        for (unsigned int i = 0; i < ids_to_purche.size(); i++) {
           request = "DELETE FROM DATA WHERE id = " + to_string(ids_to_purche[i]);
           cout << request <<endl;
           check_status(request.c_str(), db, rc, &stmt);
           sqlite3_finalize(stmt);
        }
    }   
}

