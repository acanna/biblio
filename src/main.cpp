#include <iostream>
#include "DBLPManager.h"
#include "ArticleInfo.h"
#include <list>
#include "parser.h"

using namespace std;

int main (int argc, char ** argv) {

	if (argv[1] == "A") {

		parser * pr;
		string file_name; 
		if (argc > 1){
			try{
			file_name = string(argv[1]);
			pr = new parser(file_name);
			} catch(...) {}
		} else {		
			cin >> file_name;
			pr = new parser(file_name);
	    }
	    
	    list<string> auths = pr->get_authors();
	    for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it){
	    	cout << *it << endl;
	    }
	    
	    delete pr;
	
	}
	else if (argv[1] == "Y"){
		for (int i = 2; i < argc; i++) {	
			DBLPManager dblp;
			vector <ArticleInfo> result = dblp.publicationRequest(string(argv[i]));
			for (unsigned int k = 0; k < result.size(); k++) {
				std::cout << result[k].to_String() << " \n"; 
			}
		}	
	}
  	return 0;
	}


