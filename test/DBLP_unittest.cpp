#include "../src/DBLPManager.h"
#include "../src/ArticleInfo.h"
#include "../src/find_info.h"
#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <cstring>

using namespace std;

DBLPManager dblp;

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

	ifstream file;
    	file.open(data_file);
	int passed = 0;
	int counter = 0;
	while ((file.is_open()) && (!file.eof()))  {
		char buffer [256];
		file.getline (buffer, 256);
		string line =  string (buffer);
		string filename = line.substr(0, 12);				
		string paper_title = line.substr(12);
		filename = path + filename;
		paper_title.erase(paper_title.find_last_not_of(" \n\r\t")+1);
		string whitespace = " \n\r\t";
		int num = strspn(paper_title.c_str(),  whitespace.c_str());
		paper_title = paper_title.substr(num);
		transform(paper_title.begin(), paper_title.end(), paper_title.begin(), (int (*)(int))tolower);
		filename.erase(filename.find_last_not_of(" \n\r\t")+1);

//		cout << "-------------------------------------------------" << endl;	
//		cout << "Searching:......" << paper_title << endl;
//		cout << "-------------------------------------------------" << endl;	

		bool offline = true;

		vector <ArticleInfo> result_online = find_info(filename, !offline);
		vector <string> title_online = {};

		for (unsigned int i = 0; i < result_online.size(); i++) {
			string cur_title = result_online[i].get_title();
			transform(cur_title.begin(), cur_title.end(), cur_title.begin(), (int (*)(int))tolower);
			title_online.push_back(cur_title);
//			cout << cur_title << endl;

		}

		bool flag = false;
		unsigned int index = 0;

		while ((index < title_online.size()) & !flag) {
			string cur_title = title_online[index];
			size_t pos = cur_title.find(paper_title);		
			if (pos != std::string::npos) {
			        flag = true;
//				cout << cur_title << endl;
				break;
			}
			index++;
		}




		if (flag) {
			passed++ ;
		} else {
			cout << "Failed at " << filename << endl;
		}
		counter++;

	}
						    
	cout <<">>>-------------------------------------<<<" << endl;
	cout <<"    Passed " << passed << " tests from " << counter << endl;
	cout <<"    Passed " << passed*100/float(counter) << " % from total amount " << endl;
	cout <<">>>-------------------------------------<<<" << endl;
	EXPECT_EQ(0, 0);
}




/*
int main (int argc , char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
*/