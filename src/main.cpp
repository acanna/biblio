#include <iostream>
#include "DBLPManager.h"
#include "ArticleInfo.h"

int main (int argc, char ** argv) {
	for (int i = 1; i < argc; i++) {	
		DBLPManager dblp;
		vector <ArticleInfo> result = dblp.publicationRequest(string(argv[i]));
		for (unsigned int k = 0; k < result.size(); k++) {
			std::cout << result[k].to_String() << " \n"; 
		}
	}	
	
  	return 0;
}


