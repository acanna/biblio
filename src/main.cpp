#include <iostream>
#include <algorithm>

#include <tclap/CmdLine.h>
#include <unistd.h>

#include "Requesters/Requester.h"
#include "BiblioManager.h"
#include "BiblioThreadContext.h"

using namespace std;

int main(int argc, char **argv) {
    try {
        TCLAP::CmdLine cmd("This util will generate .bib files for your articles in PDF format.", ' ', "0.1");
        TCLAP::SwitchArg offlineSwitch("f", "offline", "Does only offline part.", cmd, false);
        TCLAP::SwitchArg purgeSwitch("p", "purge", "Purges database from non-existent files.", false);
        TCLAP::SwitchArg dbSwitch("b", "database", "Disable database", cmd, false);
        TCLAP::UnlabeledMultiArg<string> files("files", "file names", true, "files");
        TCLAP::MultiArg<string> directories("d", "directory", "directories for recursive search of PDF documents", true, "path");
        TCLAP::ValueArg<string> config_file("c", "config", "name of the config file",false,"../biblio.cfg","filename");
        cmd.add(&config_file);
        vector<TCLAP::Arg*>  xorlist;
        xorlist.push_back(&purgeSwitch);
        xorlist.push_back(&files);
        xorlist.push_back(&directories);
        cmd.xorAdd(xorlist);

        cmd.parse(argc, (const char *const *) argv);
        vector<string> filenames = {};
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
        size_t filenames_size = filenames.size();
        for (size_t i = 0; i < filenames_size; i++) {
            filenames[i] = get_absolute_path(filenames[i]);
        }
        bool offline = offlineSwitch.getValue();
        bool purge = purgeSwitch.getValue();
        bool without_db = dbSwitch.getValue();
        
        if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
            throw new BiblioException("Curl global init failed.\n");
        }
		Config::init(config_file.getValue());
        int threads = sysconf(_SC_NPROCESSORS_ONLN);
        BiblioManager manager(threads);

        ofstream out_html("biblio.html");
        BiblioManager::start_print_html(out_html);
        ofstream out_bib("biblio.bib");

        if (purge) {
    		Database::purge();
        } else {	
            bool using_db = false; 
            try {
                Database * db;
    			if (!without_db){                
                    db = Database::connect_database();
        			if (db != nullptr) {
        				using_db = true;
        			}  
                }     
                vector<string> filenames_to_search = {};
    			vector<ArticleInfo> data_from_db ={};
                ArticleInfo *result_ptr = nullptr;
        
                for (const auto &filename : filenames) {	
    				if (using_db) {
    					result_ptr = db->get_data(filename);
                    } 
    				if (result_ptr != nullptr) {
                        if (! need_to_complete_data(result_ptr)) {
    					    data_from_db.push_back(*result_ptr);
                            //здесь бы сделать deleter result_ptr
                            //ведь копия уже создана
                        } else {
                            filenames_to_search.push_back(filename);
                        }
    				}
                    else if ((!using_db) || (result_ptr == nullptr)) {
                        filenames_to_search.push_back(filename);
    				}
                }
                queue<string, deque<string>> in(deque<string>(filenames.begin(), filenames.end()));
                BiblioThreadContext::init(in);
                vector<ArticleInfo> result = manager.search_distance(levenshtein_distance, offline);
    	        BiblioManager::cout_not_found_articles(result);
                manager.print_html(out_html, data_from_db);
                manager.print_bib(out_bib, data_from_db);
                manager.print_html(out_html, result);
                manager.print_bib(out_bib, result);
    
    			if ((using_db)&&(!without_db)) {
                	db->add_data(result);
    				delete db;
    			}
            } catch (const BiblioException &e) {
                cerr << e.what() << '\n';
            }
            BiblioManager::end_print_html(out_html);
            out_html.close();
            out_bib.close();          
        } 
    }
    catch (TCLAP::ArgException &e) {
            std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }    
    return 0;
}
