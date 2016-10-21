#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <exception>
#include "DBLPManager.h"
#include "ArticleInfo.h"
#include "Parser.h"
#include "find_info.h"
#include <tclap/CmdLine.h>

using namespace std;

// kaka budet vygl'adet vyzov prilozhenija? poka realizovano 
//	 ./main [--offline | -f] file1.pdf file2.pdf ...


int main (int argc, char ** argv) {
	
	// Wrap everything in a try block.  Do this every time, 
	// because exceptions will be thrown for problems.
	try {
	// Define the command line object, and insert a message
	// that describes the program. The "Command description message" 
	// is printed last in the help text. The second argument is the 
	// delimiter (usually space) and the last one is the version number. 
	// The CmdLine object parses the argv array based on the Arg objects
	// that it contains. 
	TCLAP::CmdLine cmd("This util will generate .bib files for your articles in .pdf format", ' ', "0.1");

	// Define a switch and add it to the command line.
	// A switch arg is a boolean argument and only defines a flag that
	// indicates true or false.  In this example the SwitchArg adds itself
	// to the CmdLine object as part of the constructor.  This eliminates
	// the need to call the cmd.add() method.  All args have support in
	// their constructors to add themselves directly to the CmdLine object.
	// It doesn't matter which idiom you choose, they accomplish the same thing.
	TCLAP::SwitchArg offlineSwitch("f","offline","Do only offline part", cmd, false);
	
	//
	// UnlabeledMultiArg must be the LAST argument added!
	//
	TCLAP::UnlabeledMultiArg<string> multi("file_names", "file names", true, "list of strings");
	cmd.add( multi );
	
	// Parse the argv array.
	cmd.parse(argc, argv);
	// Get the value parsed by each arg. 
	vector<string> fileNames = multi.getValue();
	
	bool offline = offlineSwitch.getValue();

	// Do what you intend. 
	for (const auto &filename : fileNames) // access by reference to avoid copying
	{
		cout << "--------------------------------------------------------------" << endl;
		cout << filename << endl;
		cout << "--------------------------------------------------------------" << endl;
		vector <ArticleInfo> result = find_info(filename, offline);
		print_info(result);

	}


	} catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	return 0;
}


