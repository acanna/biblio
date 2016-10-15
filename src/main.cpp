#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <exception>
#include "DBLPManager.h"
#include "ArticleInfo.h"
#include "Parser.h"

using namespace std;

int main (int argc, char ** argv) {
	list<string> auths, title;
	DBLPManager * dblp;
	unsigned int result_size = 0;
	if (string(argv[1]).string::compare("A") == 0) {
		Parser * pr;
		string file_name; 
		if (argc > 2) {
			file_name = string(argv[2]);
			pr = new Parser(file_name);
		} else {
			cin >> file_name;
			pr = new Parser(file_name);
		}
		auths = pr->get_authors();
		title = pr->get_title();
		cout << "------------------parser---------------------" << endl;
		cout << "------------------authors---------------------" << endl;
		for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it) {
			cout << *it << endl;
		}
		cout << "------------------title---------------------" << endl;
		for (list<string>::const_iterator it = title.begin(); it != title.end(); ++it) {
			cout << *it << endl;
		}
		cout << "------------------end-parser-----------------" << endl;
		delete pr;
	}
	else if (string(argv[1]).string::compare("B") == 0) {
		Parser * pr;
		string file_name; 
		if (argc > 2) {
			file_name = string(argv[2]);
			pr = new Parser(file_name);
		} else {
			cin >> file_name;
			pr = new Parser(file_name);
		}
		auths = pr->get_authors();
		cout << "------------------parser-----------------" << endl;
		for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it) {
			cout << *it << endl;
		}
		cout << "------------------dblp-----------------" << endl;


		try {
			dblp = new DBLPManager();
			for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it) {
				vector <ArticleInfo> result = dblp->publicationRequest(*it);
				result_size = result.size();
				for (unsigned int k = 0; k < result_size; ++k) {
					cout << result[k].to_string() << " \n"; 
				}
			}
		} 
		catch (const exception & e) {
			cout << e.what() << endl;
		
		}
		delete dblp;
		delete pr;

/***************************************************************************************************
		for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it) {
			vector <ArticleInfo> result = dblp->publicationRequest(*it);
			result_size = result.size();
			for (unsigned int k = 0; k < result_size; ++k) {
				cout << result[k].to_string() << " \n"; 
			}
		}
		delete pr;
****************************************************************************************************/

	}
	else if (string(argv[1]).string::compare("Y") == 0) {			
		try {
			dblp = new DBLPManager();
			for (int i = 2; i < argc; i++) {
				string query = string(argv[i]);
				replace(query.begin(), query.end(), ' ', '.'); 			
				vector <ArticleInfo> result = dblp->publicationRequest(query);
				result_size = result.size();
				for (unsigned int k = 0; k < result_size; ++k) {
					cout << result[k].to_string() << endl; 
				}
			}		
		} 
		catch (const exception & e) {
			cout << e.what() << endl;
		
		}
		delete dblp;
	}
	return 0;
}


