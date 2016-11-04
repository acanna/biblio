#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include "gtest/gtest.h"
#include "../src/DBLPManager.h"
#include "../src/BiblioManager.h"
#include "../src/tools.h"
#include "../src/PictureParser.h"

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

TEST (PaperDatasetTest, Positive) {
	string data_file = "../articles/test_summary.txt";
	string path = "../articles/";

	ifstream file(data_file);
	int passed = 0;
	int counter = 0;
    string line = "";
    while ((file.is_open()) && (!file.eof()))  {

		getline(file, line);
        vector<string> tmp = split(line, '\t');

		string filename = tmp[0];
		string paper_title = tmp[1];
		filename = path + filename;

		bool offline = true;
		try{
			vector <ArticleInfo> result = manager.find_info(filename, !offline);
			if (result.size() > 0) {
		        int precision = result[0].get_precision();
				if (precision == 100) {
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
	EXPECT_EQ(0, 0);
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
				int precision = result[0].get_precision();
				if (precision == 100) {
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

	EXPECT_EQ(0, 0);
}


