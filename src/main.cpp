#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>
#include "DBLPManager.h"
#include "BiblioManager.h"


using namespace std;


int main (int argc, char ** argv) {
    try {
        TCLAP::CmdLine cmd("This util will generate .bib files for your articles in .pdf format.", ' ', "0.1");
        TCLAP::SwitchArg offlineSwitch("f", "offline", "Does only offline part.", cmd, false);
        TCLAP::UnlabeledMultiArg<string> multi("files", "file names", true, "files");
        cmd.add(multi);
        // Parse the argv array.
        cmd.parse(argc, (const char *const *) argv);
        // Get the value parsed by each arg.
        vector<string> fileNames = multi.getValue();
        bool offline = offlineSwitch.getValue();

        BiblioManager manager;
        ofstream out_html("biblio.html");
        ofstream out_bib("biblio.bib");

        for (const auto & filename : fileNames) // access by reference to avoid copying
        {
            try
            {
                vector <ArticleInfo> result = manager.search_levenshtein_light(filename, offline);
                manager.print_html(out_html, filename, result);
                manager.print_bib(out_bib, result);

            } catch (const Biblio_exception & e) {
                cerr << e.what() << '\n';
            } catch (...) {}
        }

        out_html.close();
        out_bib.close();

/*		PictureParser picture_parser = PictureParser("test_1.pdf", 300, 300, "test.png", "png", 700);
		string result = picture_parser.find_title();*/

    } catch (TCLAP::ArgException & e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
    return 0;
}
