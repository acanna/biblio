#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>
<<<<<<< HEAD
#include "DBLPManager.h"
//#include "find_info.h"
#include "PictureParser.h"
#include "Biblio_exception.h"
=======
#include "BiblioManager.h"
>>>>>>> ee3ba6fee1d3a77bef027157b26b313e541224fa


using namespace std;

int main () {
/*int main (int argc, char ** argv) {
	try {
		TCLAP::CmdLine cmd("This util will generate .bib files for your articles in .pdf format.", ' ', "0.1");
		TCLAP::SwitchArg offlineSwitch("f", "offline", "Does only offline part.", cmd, false);
		TCLAP::UnlabeledMultiArg<string> multi("files", "file names", true, "files");
		cmd.add(multi);
		// Parse the argv array.
		cmd.parse(argc, argv);
		// Get the value parsed by each arg. 
		vector<string> fileNames = multi.getValue();
		bool offline = offlineSwitch.getValue();

        BiblioManager manager;
        ofstream out_html("biblio.html");
        ofstream out_bib("tex/biblio.bib");

		for (const auto & filename : fileNames) // access by reference to avoid copying
		{
			try 
			{
<<<<<<< HEAD
				vector <ArticleInfo> result = find_info(filename, offline);
				ofstream out("biblio.html");
				print_html(out, filename, result);
				ofstream out_bib("tex/biblio.bib");
				print_bib(out_bib, result);*/

                // try to parse image using Tesseract and leptonica 
	
				PictureParser picture_parser = PictureParser("test_4.pdf", 300, 300, "test.png", "png", 700);
			    picture_parser.save_as_image();


//				string out_text = picture_parser.parse_image();
//				cout << out_text;
=======
				vector <ArticleInfo> result = manager.search_exact_match(filename, offline);
				manager.print_html(out_html, filename, result);
				manager.print_bib(out_bib, result);

                // try to parse image using Tesseract and leptonica
				/*
				PictureParser picture_parser = PictureParser("test_11.pdf", 300, 300, "test_11.png", "png", 150);
				picture_parser.save_as_image(0);
				char* out_text = picture_parser.parse_image();
				cout << out_text;
				delete [] out_text;
				*/
>>>>>>> ee3ba6fee1d3a77bef027157b26b313e541224fa
				
	/*			
			} catch (const Biblio_exception & e) {
				cerr << e.what() << '\n'; 
			} catch (...) {}
		}

        out_html.close();
        out_bib.close();

	} catch (TCLAP::ArgException & e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}*/
	return 0;
}


