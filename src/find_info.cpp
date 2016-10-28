#include "find_info.h"
#include <regex>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;


bool _greater (const ArticleInfo& info_1, const ArticleInfo& info_2) {
     return (info_1.get_precision() > info_2.get_precision());
}

string delete_junk_symbol(string& str) {
	transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
    replace(str.begin(), str.end(), '.', ' ');
    str.erase(str.find_last_not_of(" \n\r\t")+1);
    return str;
}

vector <ArticleInfo> search_dblp (DBLPManager& dblp, string query) {
    vector <ArticleInfo> result = {};

    vector <ArticleInfo> additional_result = {};

    string new_query = query;
    transform(new_query.begin(), new_query.end(), new_query.begin(), (int (*)(int))tolower);
    result = dblp.publicationRequest(new_query);

    replace(new_query.begin(), new_query.end(), ' ', '.');
    additional_result = dblp.publicationRequest(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    transform(query.begin(), query.end(), query.begin(), (int (*)(int))tolower);
    replace(query.begin(), query.end(), ' ', '$');
    additional_result = dblp.publicationRequest(query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());
    
    return result;
}

int levenshtein_distance(string &s, string &t) {
   
    if (s == t) return 0;
    if (s.length() == 0) return t.length();
    if (t.length() == 0) return s.length();

    int v0[t.length()+1];
    int v1[t.length()+1];
    for (unsigned int i = 0; i < t.length() + 1; i++) {
        v0[i] = i;
    }
    for (unsigned int i = 0; i < s.length(); i++){
        v1[0] = i + 1;
        for (unsigned int j = 0; j < t.length(); j++) {
            int cost = (s[i] == t[j]) ? 0 : 1;
            v1[j + 1] = min(v1[j] + 1, min(v0[j + 1] + 1, v0[j] + cost));
        }
        for (unsigned int j = 0; j < t.length() + 1; j++)
            v0[j] = v1[j];
    }
    return v1[t.length()];
}

vector <ArticleInfo> find_info(const string & filename, bool offline) {
	list<string> auths_candidates, title_candidates;
	vector <ArticleInfo> result = {};
	vector <ArticleInfo> dblp_result = {};
	try {
		Parser pr = Parser (filename);
		auths_candidates = pr.get_authors();
		title_candidates = pr.get_title();
	} catch (const Biblio_exception & e) {
		throw;
	}
	
	string data_from_parser="";
	string title = "";
	vector <string> authors = {};


	// info from parser
	// title
	for (list<string>::const_iterator it = title_candidates.begin(); it != title_candidates.end(); ++it) {
		data_from_parser += (string) (*it) + " ";
        title += (string) (*it) + " ";
	}
    // authors
	for (list<string>::const_iterator it = auths_candidates.begin(); it != auths_candidates.end(); ++it) {
        string aut = (string)(*it);
		authors.push_back(aut);
		data_from_parser += aut;
	}

	// dblp
	if (!offline) {
		try {
            // all data from parser
			DBLPManager dblp = DBLPManager();
			result = search_dblp(dblp, data_from_parser);
		
            // title only 
			dblp_result = search_dblp(dblp, title);
			result.insert(result.end(), dblp_result.begin(), dblp_result.end());

            // authors only 
            for (list<string>::const_iterator it = auths_candidates.begin(); 
                                it != auths_candidates.end(); ++it) {
 				string query = (string) (*it);
    			dblp_result = search_dblp(dblp, query);
				result.insert(result.end(), dblp_result.begin(), dblp_result.end());
  			}
		} 
		catch (const exception & e) {
			cerr << e.what() << endl;
		}            
         if (result.size()>0) {
            data_from_parser = delete_junk_symbol(data_from_parser);
            for (unsigned int i = 0; i < result.size(); i++) {
    			string cur_title = result[i].get_title();
                cur_title = delete_junk_symbol(cur_title);
                bool the_same = false; 
                if (data_from_parser.find(cur_title) != std::string::npos) {
                   the_same = true;     
                }
                if (the_same) {
                    result[i].set_precision(100);
                }
                else {
                    int lev_distance = levenshtein_distance(cur_title, data_from_parser);
                    result[i].set_precision(100-(int)(100*lev_distance/data_from_parser.size()));
                }
            }
      
            sort(result.begin(), result.end(), _greater);    
        
        //    if (result[0].get_precision() == 100) {
        //       result.resize(1);
        //    } else {
        //       result.resize(5); 
        //    }
        }
	}
	return result;
}

void print_info(vector <ArticleInfo> & result) {

	unsigned int result_size = result.size();

	for (unsigned int k = 0; k < result_size; ++k) {
		cout << result[k].to_string() << " \n"; 
	}
}

void printf_info(const string & filename, vector <ArticleInfo> & result) {
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

