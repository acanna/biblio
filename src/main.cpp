#include <iostream>
#include <list>
#include "DBLPManager.h"
#include "ArticleInfo.h"
#include "parser.h"

using namespace std;

int main (int argc, char ** argv) {
	list<string> auths;
	DBLPManager dblp;
	int result_size = 0;
	if (argv[1] == "A") {
		parser * pr;
		string file_name; 
		if (argc > 2){
			try {
				file_name = string(argv[2]);
				pr = new parser(file_name);
			} catch(...) {}
		} else {
			cin >> file_name;
			pr = new parser(file_name);
		}
		auths = pr->get_authors();
		cout << "------------------parser-----------------" << endl;
		for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it) {
			cout << *it << endl;
		}
		cout << "------------------dblp-----------------" << endl;
		for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it) {
			vector <ArticleInfo> result = dblp.publicationRequest(*it);
			result_size = result.size();
			for (unsigned int k = 0; k < result_size; ++k) {
				cout << result[k].to_String() << " \n"; 
			}
		}
		delete pr;
	}
	else if (argv[1] == "Y"){
		for (int i = 2; i < argc; i++) {			
			vector <ArticleInfo> result = dblp.publicationRequest(string(argv[i]));
			result_size = result.size();
			for (unsigned int k = 0; k < result_size; ++k) {
				cout << result[k].to_String() << " \n"; 
			}
		}
	}
	return 0;
}


