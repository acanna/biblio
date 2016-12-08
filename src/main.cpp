#include <iostream>
#include <algorithm>

#include <tclap/CmdLine.h>

#include "Requesters/Requester.h"
#include "BiblioManager.h"
#include "Database.h"

using namespace std;

int main(int argc, char **argv) {
    try {
        TCLAP::CmdLine cmd("This util will generate .bib files for your articles in PDF format.", ' ', "0.1");
        TCLAP::SwitchArg offlineSwitch("f", "offline", "Does only offline part.", cmd, false);
        TCLAP::UnlabeledMultiArg<string> files("files", "file names", true, "files");
        TCLAP::MultiArg<string> directories("d", "directory", "directories for recursive search of PDF documents", true, "path");
        // User can input files or directories but not both.
        cmd.xorAdd(files, directories);
        // Parse the argv array.
        cmd.parse(argc, (const char *const *) argv);
        vector<string> filenames = {};
        // Get the value parsed by each arg.
        if (files.isSet()) {
            filenames = files.getValue();
        }
        else {
            vector<string> dirs = directories.getValue();
            vector<string> files_in_dir = {};
            for (string dir : dirs) {
                files_in_dir = read_pdf_files_recursive(dir);
                filenames.insert(filenames.end(), files_in_dir.begin(), files_in_dir.end());
            }
        }
        bool offline = offlineSwitch.getValue();
        
        if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
            throw new BiblioException("Curl global init failed.\n");
        }

        int threads = 4;

		Config::init("../biblio.cfg");

        BiblioManager manager(threads);

        ofstream out_html("biblio.html");
        ofstream out_bib("biblio.bib");

        try {
        	bool using_db = false;
			Database * db = Database::connect_database();
			if (db != nullptr) {
				using_db = true;
			}
			
            vector<string> filenames_to_search = {};
			vector<ArticleInfo> data_from_db ={};
            ArticleInfo *result_ptr;

            for (const auto &filename : filenames) {	
				if (using_db) {
					result_ptr = db->get_data(filename);
                } 
				if (result_ptr != nullptr){
					data_from_db.push_back(*result_ptr);
				}
                else if ((!using_db) || (result_ptr == nullptr)) {
                    filenames_to_search.push_back(filename);
				}
            }

            vector<ArticleInfo> result = manager.search_distance(levenshtein_distance, filenames_to_search, offline);
	        manager.print_html(out_html, data_from_db);
            manager.print_bib(out_bib, data_from_db);
            manager.print_html(out_html, result);
            manager.print_bib(out_bib, result);
			if (using_db) {
            	db->add_data(result);
				delete db;
	            delete result_ptr;
			}
        } catch (const BiblioException &e) {
            cerr << e.what() << '\n';
        }

        out_html.close();
        out_bib.close();

    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    return 0;
}
