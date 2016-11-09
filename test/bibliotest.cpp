#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include "gtest/gtest.h"
#include "../src/DBLPManager.h"
#include "../src/BiblioManager.h"
#include "../src/tools.h"
#include "../src/PictureParser.h"
#include "../src/Biblio_exception.h"
#include "../src/Parser.h"

using namespace std;

DBLPManager dblp;
BiblioManager manager;

TEST (PaperPresent, Positive) {
	const string query = "Land.Cover.Classification.and.Forest.Change.Analysis";
	const string title = "Land Cover Classification and Forest Change Analysis, Using Satellite Imagery-A Case Study in Dehdez Area of Zagros Mountain in Iran.";
	const string venue = "J. Geographic Information System";
	const string year = "2011";
	vector <ArticleInfo> result = dblp.publicationRequest(query);
	EXPECT_EQ(result[0].get_year(), year);
	EXPECT_EQ(result[0].get_venue(), venue);
	EXPECT_EQ(result[0].get_title(), title); 
}

TEST (PaperAbsent, Negative) {
	const string query = "QQQ";
	vector <ArticleInfo> result = dblp.publicationRequest(query);
	EXPECT_EQ(result.size(), 0);
}

TEST (WrongQuery, Negative) {
	const string query = "http://dblp.org/search/pbl/api?q";
	vector <ArticleInfo> result = dblp.publicationRequest(query);
	EXPECT_EQ(result.size(), 0);
}

TEST (TestAlg_TitleExactMatch, Positive) {
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

		bool offline = false;
		try{
			vector <ArticleInfo> result = manager.search_exact_match(filename, offline);
			if (result.size() > 0) {

				if (low_letters_only(paper_title) == low_letters_only(result[0].get_title())) {
					passed++ ;
				} else {
					cout << "Failed at " << filename << endl;
				}
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

	EXPECT_EQ(passed, counter);
}

TEST (TestAlg_TitleLevenshtein, Positive) {

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

		bool offline = false;
		try{
			vector <ArticleInfo> result = manager.search_levenshtein(filename, offline);
			if (result.size() > 0) {

				if (low_letters_only(paper_title) == low_letters_only(result[0].get_title())) {
					passed++ ;
				} else {
					cout << "Failed at " << filename << endl;
				}
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

	EXPECT_EQ(passed, counter);
}

TEST (TestAlg_TitleDamerauLevenshtein, Positive) {

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

		bool offline = false;
		try{
			vector <ArticleInfo> result = manager.search_damerau_levenshtein(filename, offline);
			if (result.size() > 0) {

				if (low_letters_only(paper_title) == low_letters_only(result[0].get_title())) {
					passed++ ;
				} else {
					cout << "Failed at " << filename << endl;
				}
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

    EXPECT_EQ(passed, counter);
}

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





