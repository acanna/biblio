#include "find_info.h"
#include <regex>
#include <fstream>
using namespace std;

vector <ArticleInfo> find_info(string filename, bool offline) {
	list<string> auths_candidates, title_candidates;
	DBLPManager dblp;
	vector <ArticleInfo> result = {};
	vector <ArticleInfo> dblp_exact_result = {};
	vector <ArticleInfo> dblp_tentative_result = {};
	try {
		Parser pr = Parser (filename);
		auths_candidates = pr.get_authors();
		title_candidates = pr.get_title();
	} catch (const Biblio_file_exception & e) {
		throw;
	}
	
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
				//dblp_tentative_result = dblp.publicationRequest(query);
				replace(query.begin(), query.end(), ' ', '.');
				dblp_exact_result = dblp.publicationRequest(query);
				result.insert(result.end(), dblp_exact_result.begin(), dblp_exact_result.end());
				//result.insert(result.end(), dblp_tentative_result.begin(), dblp_tentative_result.end());
			}
			result.insert(result.end(), dblp_exact_result.begin(), dblp_exact_result.end());
			//result.insert(result.end(), dblp_tentative_result.begin(), dblp_tentative_result.end());
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

void printf_info(string filename, vector <ArticleInfo> result) {
	unsigned int result_size = result.size();
	ofstream out("result.html");
	out << "<html>\n";
	out << "\t<head>\n";
	out << "\t\t<title>Biblio results</title>\n";
	out << "\t</head>\n";
	out << "\t<body>\n";
	out << "--------------------------------------------------------------" << endl;
	out << "<br>\n";
	out << filename << endl;
	out << "<br>\n";
	out << "--------------------------------------------------------------" << endl;
	out << "<br>\n";
	for (unsigned int k = 0; k < result_size; ++k) {
		string frmt_str = "<br>";
		regex re_frmt("\n");
		string formatted = regex_replace(result[k].to_string(),re_frmt,frmt_str);
		out << formatted; 
		out << "<br>\n";
	}
	out << "\t</body>\n";
	out << "</html>\n";
}

