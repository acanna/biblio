#include <iostream>
#include <algorithm>

#include <tclap/CmdLine.h>
#include <unistd.h>

#include "Requesters/Requester.h"
#include "BiblioManager.h"

using namespace std;

int main(int argc, char **argv) {
    try {
        TCLAP::CmdLine cmd("This util will generate .bib file for your articles in PDF format.", ' ', "0.1");
        TCLAP::SwitchArg offlineSwitch("f", "offline", "Does only offline part.", cmd, false);
        TCLAP::SwitchArg purgeSwitch("p", "purge", "Purges database from non-existent files.", cmd, false);
        TCLAP::UnlabeledMultiArg<string> files("files", "Names of PDF files you want to get bibliographic information for.", true, "files");
        TCLAP::MultiArg<string> directories("d", "directory", "Directories for recursive search of PDF documents.", true, "path");
        // User can input files or directories but not both.
        cmd.xorAdd(files, directories);
        // Parse the argv array.
        cmd.parse(argc, (const char *const *) argv);
        vector<string> filenames = {};
        string absolut_path = get_exe_path() + "/";
        // Get the value parsed by each arg.
        if (files.isSet()) {
            filenames = files.getValue();
            size_t filenames_size = filenames.size();
            for (size_t i = 0; i < filenames_size; i++) {
                if (filenames[i][0] != '/') {
                    filenames[i] = absolut_path + filenames[i];
                }
            }
        }
        else {
            vector<string> dirs = directories.getValue();
            vector<string> files_in_dir = {};
            for (string dir : dirs) {
                if (dir[0] != '/') {
                    dir = absolut_path + dir;
                }
                files_in_dir = read_pdf_files_recursive(dir);
                filenames.insert(filenames.end(), files_in_dir.begin(), files_in_dir.end());
            }
        }
        bool offline = offlineSwitch.getValue();
        bool purge = purgeSwitch.getValue();
        
        if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
            throw new BiblioException("Curl global init failed.\n");
        }

        int threads = sysconf(_SC_NPROCESSORS_ONLN);
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
