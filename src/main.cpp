#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <exception>
#include <tclap/CmdLine.h>
#include "DBLPManager.h"
#include "ArticleInfo.h"
#include "Parser.h"
#include "find_info.h"

using namespace std;

int main (int argc, char ** argv) {
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
		for (const auto &filename : fileNames) // access by reference to avoid copying
		{
			try 
			{
				vector <ArticleInfo> result = find_info(filename, offline);
				printf_info(filename, result);
			} catch (const Biblio_exception & e) {
				cerr << e.what() << '\n'; 
			} catch (...) {}
		}
	} catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	return 0;
}


