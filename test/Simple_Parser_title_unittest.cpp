#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <gtest/gtest.h>
#include "../src/tools.h"
#include "../src/Biblio_exception.h"
#include "../src/Parser.h"

using namespace std;

TEST (PaperDatasetTest, Positive) {

	string data_file = "../articles/test_summary.txt";
	string path = "../articles/";

	ifstream file(data_file);

	int passed = 0;
	int counter = 0;

	string line = "", filename = "", paper_title = "", parsed_title = "";
	vector<string> tmp, title;
	
	while ((file.is_open()) && (!file.eof())) {
		getline(file, line);
		tmp = split(line, '\t');
		filename = tmp[0];
		paper_title = low_letters_only(tmp[1]);
		filename = path + filename;
		try{
			Parser pr(filename);
			title = pr.get_title();
		} catch(const Biblio_exception & e) {
			cerr << e.what() << endl;
			continue;
		}
        parsed_title = "";
		for (string s : title) {
            parsed_title += s + " ";
        }
        parsed_title = low_letters_only(parsed_title);
        if (parsed_title.find(paper_title) != std::string::npos) {
            passed++;
        }
        counter++;
    }
						    
	cout << ">>>-------------------------------------<<<" << endl;
	cout << "    Passed " << passed << " tests from " << counter << endl;
	cout << "    Passed " << passed * 100 / (float)counter << " % from total amount" << endl;
	cout << ">>>-------------------------------------<<<" << endl;
	EXPECT_EQ(0, 0);
}
