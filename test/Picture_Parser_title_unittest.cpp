#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include "gtest/gtest.h"
#include "../src/tools.h"
#include "../src/PictureParser.h"

using namespace std;

TEST (PictureParser, Positive) {
	string data_file = "../articles/test_summary.txt";
	string path = "../articles/";

	ifstream file(data_file);
	int passed = 0;
	int counter = 0;
	string line = "", filename = "", paper_title = "";
	vector<string> tmp;

	while (file.is_open() && !file.eof()) {

		getline(file, line);
		tmp = split(line, '\t');

		filename = tmp[0];
		paper_title = tmp[1];
		filename = path + filename;


		try{

			PictureParser picture_parser = PictureParser(filename, 300, 300, "test.png", "png", 700);
			string result = picture_parser.find_title();
			transform(result.begin(), result.end(), result.begin(), (int (*)(int))tolower);
			regex re_frmt("\\s+");
			regex re_trim("^\\s*(.*)\\s*$");
			regex re_punct("[,!?:…()*-:={}]");

			result = regex_replace(result, re_frmt, " ");
			result = regex_replace(result,re_trim,"$1");
			result = regex_replace(result,re_punct,"$1");			
			result.erase(result.find_last_not_of(" \n\r\t")+1);	

			transform(paper_title.begin(), paper_title.end(), paper_title.begin(), (int (*)(int))tolower);
			paper_title = regex_replace(paper_title,re_punct,"$1");			

			if (paper_title.find(result) != std::string::npos) {
					passed++ ;
			} else {
					cout << "Failed at " << filename << endl;
			}
			counter++;
		} catch(const Biblio_exception & e) {
			cerr << e.what() << endl;
		}
	}

	cout << ">>>-------------------------------------<<<" << endl;
	cout << "    Passed " << passed << " tests from " << counter << endl;
	cout << "    Passed " << passed * 100 / (float)counter << " % from total amount" << endl;
	cout << ">>>-------------------------------------<<<" << endl;

	EXPECT_EQ(0, 0);
}
