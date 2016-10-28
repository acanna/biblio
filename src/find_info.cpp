#include <regex>
#include "find_info.h"

using namespace std;


bool greater (const ArticleInfo& info_1, const ArticleInfo& info_2) {
     return (info_1.get_precision() > info_2.get_precision());
}

vector <ArticleInfo> find_info(const string & filename, bool offline) {
	list<string> auths_candidates, title_candidates;
	//DBLPManager dblp;
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
//	string venue ="";
//	string volume ="";
//	string number ="";
//	string pages ="";
//	string year ="";
//	string type ="";
//	string url ="";

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
            cout << "data_from_parser" << endl;
		
/*            // title only 
			dblp_result = search_dblp(dblp, title);
			result.insert(result.end(), dblp_result.begin(), dblp_result.end());
            cout << "title" << endl;
*/
            // authors only 
            for (list<string>::const_iterator it = auths_candidates.begin(); 
                                it != auths_candidates.end(); ++it) {
                cout << "authors" << endl;
 				string query = (string) (*it);
    			dblp_result = search_dblp(dblp, query);
				result.insert(result.end(), dblp_result.begin(), dblp_result.end());
  			}
		} 
		catch (const exception & e) {
			cerr << e.what() << endl;
		}
         
        if (result.size() > 0) {
            for (unsigned int i = 0; i < result.size(); i++) {
                transform(data_from_parser.begin(), data_from_parser.end(), 
                                data_from_parser.begin(), (int (*)(int))tolower);
    			string cur_title = result[i].get_title();
    			transform(cur_title.begin(), cur_title.end(), cur_title.begin(), (int (*)(int))tolower);
                bool the_same = false; 
                if (data_from_parser.find(cur_title) != std::string::npos) {
                   the_same = true;     
                }
                if (the_same) {
                    result[i].set_precision(100);
                }
                if (! the_same) {
                    int lev_distance = 10;
//                    int lev_distance = levenshtein_distance(cur_title, data_from_parser);
                    result[i].set_precision(100-(int)(100*lev_distance/data_from_parser.size()));
                }
            }
        
         }

//      sort(result.begin(), result.end(), greater());    
	}
	return result;
}


vector <ArticleInfo> search_dblp (DBLPManager & dblp, string & query) {
    vector <ArticleInfo> result = {};
    vector <ArticleInfo> additional_result = {};

    string new_query = query;
    transform(new_query.begin(), new_query.end(), new_query.begin(), (int (*)(int))tolower);
    result = dblp.publicationRequest(new_query);

    replace(query.begin(), query.end(), ' ', '.');
    additional_result = dblp.publicationRequest(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    replace(query.begin(), query.end(), ' ', '$');
    additional_result = dblp.publicationRequest(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());
    
    return result;
}


int levenshtein_distance(string s, string t) {
    // degenerate cases
    if (s == t) return 0;
    if (s.length() == 0) return t.length();
    if (t.length() == 0) return s.length();

    // create two work vectors of integer distances
    
    int v0[512];
    int v1[512];

    // initialize v0 (the previous row of distances)
    // this row is A[0][i]: edit distance for an empty s
    // the distance is just the number of characters to delete from t
    for (int i = 0; i < 512; i++) {
        v0[i] = i;
    }
    for (int i = 0; i < s.length(); i++){
        // calculate v1 (current row distances) from the previous row v0
        // first element of v1 is A[i+1][0]
        //   edit distance is delete (i+1) chars from s to match empty t
        v1[0] = i + 1;
        // use formula to fill in the rest of the row
        for (int j = 0; j < t.length(); j++) {
            int cost = (s[i] == t[j]) ? 0 : 1;
            v1[j + 1] = min(v1[j] + 1, min(v0[j + 1] + 1, v0[j] + cost));
        }
        // copy v1 (current row) to v0 (previous row) for next iteration
        for (int j = 0; j < 512; j++)
            v0[j] = v1[j];
    }
    return v1[t.size()];
}

void print_txt(ostream & out, const string & filename, vector <ArticleInfo> & result) {

	size_t result_size = result.size();
	out << "--------------------------------------------------------------" << endl;
	out << filename << endl;
	out << "--------------------------------------------------------------" << endl;
	for (size_t k = 0; k < result_size; ++k) {
		out << result[k].to_string() << " \n"; 
	}
}

void print_html(ostream & out, const string & filename, vector <ArticleInfo> & result) {
	size_t result_size = result.size();
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
	out << "<pre>\n";
	for (size_t k = 0; k < result_size; ++k) {
		out << result[k].to_string() << " \n"; 
	}
	out << "</pre>\n";
	out << "\t</body>\n";
	out << "</html>\n";
}

void print_bib(ostream & out, const string & filename, vector <ArticleInfo> & result) {
	size_t result_size = result.size();
	
}







