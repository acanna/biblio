#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>
#include "Requester.h"
#include "BiblioManager.h"
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
        BiblioManager manager;

        ofstream out_html("biblio.html");
        ofstream out_bib("biblio.bib");

        for (const auto &filename : filenames)
        {
            try {
                vector<Requester *> requesters = read_config("../biblio.cfg");
                vector<ArticleInfo> result = manager.search_distance_requesters(requesters, levenshtein_distance, filename, offline);
                manager.print_html(out_html, filename, result);
                manager.print_bib(out_bib, result);
            } catch (const Biblio_exception &e) {
                cerr << e.what() << '\n';
            } catch (...) {}
        }

        out_html.close();
        out_bib.close();

    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
    return 0;
}
