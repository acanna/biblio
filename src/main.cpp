#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <exception>
#include "DBLPManager.h"
#include "ArticleInfo.h"
#include "Parser.h"

using namespace std;

vector <ArticleInfo> find_info(string filename, string key_online) {
	list<string> auths, title;
	DBLPManager dblp;
	unsigned int result_size = 0;
	vector <ArticleInfo> result;
	Parser pr = Parser (filename);
	
	auths = pr.get_authors();
	title = pr.get_title();

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
	if (key_online.string::compare("online") == 0) {
// вопрос: может лучше искать в dblp по названию статьи? 
// С авторами все менее однозначно. Или и по аторам, и по названию.
// Название статьи, ИМХО, более уникально.
		cout << "------------------dblp-----------------" << endl;
		try {
			DBLPManager dblp = DBLPManager();
			for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it) {
 				string query = (string) (*it);
				replace(query.begin(), query.end(), ' ', '.'); 	
				result = dblp.publicationRequest(query);
				result_size = result.size();

				for (unsigned int k = 0; k < result_size; ++k) {
					cout << result[k].to_string() << " \n"; 
				}
			}
		} 
		catch (const exception & e) {
			cout << e.what() << endl;
		
		}
	}
	return result;
}

// как будет выглядеть вызов нашего приложения? Пока реализовано
//	 ./main offline file1.pdf file2.pdf  
// или 
//	 ./main online file1.pdf file2.pdf  

int main (int argc, char ** argv) {
	string filename;
	string key_online = argv[1];
	int index = 2;
        while (index < argc) {
		filename = string(argv[index]);
		find_info(filename, key_online);
		index++;
	}
	return 0;
}


