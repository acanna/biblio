#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>
#include "Requester.h"
#include "BiblioManager.h"
#include "Database.h"
#include "tools.h"

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
        int threads = 1;
        std::vector<std::pair<requestersEnum, std::vector<std::string>>> data = read_config_data("../biblio.cfg", threads);
        BiblioManager manager(threads);

        ofstream out_html("biblio.html");
        ofstream out_bib("biblio.bib");



                //1. Дважды читать конфиг это странно
                //2. Лучше уж сделать класс для конфига
                //Лучше всего его сделать по паттерну Singletone,
                //тогда не надо будет везде передавать
                //3. нет delete для db
                //4. функцию лучше спрятать в класс Database

                //EL не вижу delete для реквестеров
                //А вот, если сделать RequesterManager с деструктором, то об
                //этом можно было не думать, это были бы его проблемы
                //Можно использовать паттерн Fabric для RequesterManager
        try {
            Database *db = connect_database("../biblio.cfg");
            vector<string> filenames_to_search = {};
            ArticleInfo * result_ptr;
            for (const auto &filename : filenames)
            {
                result_ptr = db->get_data(filename);
                if (result_ptr == nullptr) {
                    filenames_to_search.push_back(filename);
                }
            }
            delete result_ptr;
            vector<ArticleInfo> result = manager.search_distance_data(data, levenshtein_distance, filenames_to_search, offline);

            manager.print_html(out_html, result);
            manager.print_bib(out_bib, result);
            db->add_data(result);
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
