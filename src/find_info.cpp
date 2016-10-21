#include "find_info.h"

using namespace std;

vector <ArticleInfo> find_info(string filename, bool offline) {
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
	if (!offline) {
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
