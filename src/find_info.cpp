#include "find_info.h"

using namespace std;

vector <ArticleInfo> find_info(string filename, bool offline) {
	list<string> auths_candidates, title_candidates;
	DBLPManager dblp;
	vector <ArticleInfo> result = {};
	vector <ArticleInfo> dblp_exact_result = {};
	vector <ArticleInfo> dblp_tentative_result = {};

	Parser pr = Parser (filename);
	
	auths_candidates = pr.get_authors();
	title_candidates = pr.get_title();

	string title ="";
	vector <string> authors = {};
	string venue ="";
	string volume ="";
	string number ="";
	string pages ="";
	string year ="";
	string type ="";
	string url ="";

	// authors
	for (list<string>::const_iterator it = auths_candidates.begin(); it != auths_candidates.end(); ++it) {
		string aut = (string)(*it);
		authors.push_back(aut);
	}
	// title
	for (list<string>::const_iterator it = title_candidates.begin(); it != title_candidates.end(); ++it) {
		title += (string) (*it) + " ";
	}
	result.push_back(ArticleInfo(title, authors, venue, volume, number, pages, year, type, url));
	
	// dblp
	if (!offline) {
		try {
			DBLPManager dblp = DBLPManager();
			for (list<string>::const_iterator it = auths_candidates.begin(); it != auths_candidates.end(); ++it) {
 				string query = (string) (*it);
				dblp_tentative_result = dblp.publicationRequest(query);
				replace(query.begin(), query.end(), ' ', '.'); 										dblp_tentative_result = dblp.publicationRequest(query);
				result.insert(result.end(), dblp_exact_result.begin(), dblp_exact_result.end());
				result.insert(result.end(), dblp_tentative_result.begin(), dblp_tentative_result.end());
			}
			result.insert(result.end(), dblp_exact_result.begin(), dblp_exact_result.end());
			result.insert(result.end(), dblp_tentative_result.begin(), dblp_tentative_result.end());
		} 
		catch (const exception & e) {
			cout << e.what() << endl;	
		}
	}

	return result;
}

void print_info(vector <ArticleInfo> result) {
	unsigned int result_size = result.size();

	for (unsigned int k = 0; k < result_size; ++k) {
		cout << result[k].to_string() << " \n"; 
	}
}