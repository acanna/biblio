#include <algorithm>
#include "BiblioManager.h"
#include "tools.h"


using namespace std;

std::vector<ArticleInfo> BiblioManager::search_exact_match(const std::string &filename, bool offline) {

    vector<string> title_candidates = {};
    vector <ArticleInfo> result = {};
    vector <ArticleInfo> dblp_result = {};

    try {
        parser = Parser(filename);
        title_candidates = parser.get_title();
        size_t n = title_candidates.size();
        for (size_t i = 0; i < n - 1; ++i) {
            title_candidates.push_back(title_candidates[i] + " " + title_candidates[i + 1]);
        }
    } catch (const Biblio_exception & e) {
        throw;
    }

    string title = "";
    for (string s : title_candidates) {
        title += s + " ";
    }

    if (!offline) {
        try {
            string prep_title = low_letters_only(title);

            for (string & s : title_candidates) {

                dblp_result = search_dblp(requester, s);

                if (dblp_result.size() > 0) {

                    size_t result_size = dblp_result.size();

                    for (size_t i = 0; i < result_size; i++) {

                        string cur_title = dblp_result[i].get_title();
                        cur_title = low_letters_only(cur_title);

                        if (prep_title.find(cur_title) != std::string::npos) {

                            dblp_result[i].set_precision(100);
                            vector<ArticleInfo> match;
                            match.push_back(dblp_result[i]);
                            return match;

                        } else {
                            int lev_distance = levenshtein_distance(cur_title, prep_title);
                            dblp_result[i].set_precision(100 - (int) (100 * lev_distance / prep_title.size()));
                        }

                    }

                    result.insert(result.end(), dblp_result.begin(), dblp_result.end());
                }
            }
            sort(result.begin(), result.end(), greater);
        }
        catch (const Biblio_exception & e) {
            throw;
        }
    }
    return result;
}

std::vector<ArticleInfo> BiblioManager::search_dblp(DBLPManager & dblp, std::string & query) {

    vector <ArticleInfo> result = {};

    vector <ArticleInfo> additional_result = {};

    query = delete_multiple_spaces(query);

    string new_query = query;
    transform(new_query.begin(), new_query.end(), new_query.begin(), ::tolower);
    result = dblp.publicationRequest(new_query);

    replace(new_query.begin(), new_query.end(), ' ', '.');
    additional_result = dblp.publicationRequest(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    transform(query.begin(), query.end(), query.begin(), ::tolower);
    replace(query.begin(), query.end(), ' ', '$');
    additional_result = dblp.publicationRequest(query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    return result;
}

bool BiblioManager::greater(const ArticleInfo &info_1, const ArticleInfo &info_2) {
    return (info_1.get_precision() > info_2.get_precision());
}

BiblioManager::BiblioManager() {  }

void BiblioManager::print_txt(std::ostream &out, const std::string &filename, std::vector<ArticleInfo> &result) {

    size_t result_size = result.size();
    out << "--------------------------------------------------------------" << endl;
    out << filename << endl;
    out << "--------------------------------------------------------------" << endl;
    for (size_t k = 0; k < result_size; ++k) {
        out << result[k].to_string() << " \n";
    }
}

void BiblioManager::print_bib(std::ostream &out, std::vector<ArticleInfo> &result) {

    size_t result_size = result.size();
    for (size_t k = 0; k < result_size; ++k) {
        vector<string> authors = result[k].get_authors();
        size_t t = authors.size();
        string label = "";

        if (t > 1) {
            label = short_name(authors[0]) + short_name(authors[1]) + result[k].get_year();
        } else {
            label = short_name(authors[0]) + result[k].get_year();
        }

        out <<  "@ARTICLE{" << label << ",\n";
        out << "author = {";
        for (size_t i = 0; i < t - 1; ++i) {
            out << authors[i] << ", ";
        }
        out << authors[t - 1];
        out << "},\n";
        out << "title = {" << result[k].get_title() << "}";

        if (result[k].get_pages().size() > 0) {
            out << ",\n" << "pages = {" << result[k].get_pages() << "}";
        }
        if (result[k].get_number().size() > 0) {
            out << ",\n" << "number = {" << result[k].get_number() << "}";
        }
        if (result[k].get_venue().size() > 0) {
            out << ",\n" << "venue = {" << result[k].get_venue() << "}";
        }
        if (result[k].get_type().size() > 0) {
            out << ",\n" << "type = {" << result[k].get_type() << "}";
        }
        if (result[k].get_url().size() > 0) {
            out << ",\n" << "url = {" << result[k].get_url() << "}";
        }
        if (result[k].get_volume().size() > 0) {
            out << ",\n" << "volume = {" << result[k].get_volume() << "}";
        }
        if (result[k].get_year().size() > 0) {
            out << ",\n" << "year = {" << result[k].get_year() << "}";
        }
        out << "\n}\n\n";
    }
}

void BiblioManager::print_html(std::ostream &out, const std::string &filename, std::vector<ArticleInfo> &result) {

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

vector<ArticleInfo> BiblioManager::find_info(const string & filename, bool offline) {

    list<string> auths_candidates;
    vector<string> title_candidates;

    vector <ArticleInfo> result = {};
    vector <ArticleInfo> dblp_result = {};

    try {
        parser = Parser(filename);
        auths_candidates = parser.get_authors();
        title_candidates = parser.get_title();
    } catch (const Biblio_exception & e) {
        throw;
    }

    string data_from_parser = "";
    string title = "";
    vector <string> authors = {};

    // info from parser
    // title
    for (string s : title_candidates) {
        data_from_parser += s + " ";
        title += s + " ";
    }
    // authors
    for (list<string>::const_iterator it = auths_candidates.begin(); it != auths_candidates.end(); ++it) {
        string aut = (*it);
        authors.push_back(aut);
        data_from_parser += aut;
    }

    // dblp
    if (!offline) {
        try {
            /*
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
             */

            string prep_title = low_letters_only(title);
            for (string & s : title_candidates) {
                dblp_result = search_dblp(requester, s);
                if (dblp_result.size() > 0) {
                    size_t result_size = dblp_result.size();
                    for (size_t i = 0; i < result_size; i++) {
                        string cur_title = dblp_result[i].get_title();
                        cur_title = low_letters_only(cur_title);
                        bool the_same = false;
                        if (prep_title.find(cur_title) != std::string::npos) {
                            the_same = true;
                        }
                        if (the_same) {
                            dblp_result[i].set_precision(100);
                            vector<ArticleInfo> match;
                            match.push_back(dblp_result[i]);
                            return match;
                        } else {
                            int lev_distance = levenshtein_distance(cur_title, prep_title);
                            dblp_result[i].set_precision(100 - (int) (100 * lev_distance / prep_title.size()));
                        }
                    }
                    result.insert(result.end(), dblp_result.begin(), dblp_result.end());
                }
            }
            sort(result.begin(), result.end(), greater);


        }
        catch (const Biblio_exception & e) {
            throw;
        }

        if (result.size() > 0) {
            //data_from_parser = delete_junk_symbol(data_from_parser);
            string prep_title = low_letters_only(title);
            size_t result_size = result.size();
            for (size_t i = 0; i < result_size; i++) {
                string cur_title = result[i].get_title();
                //cur_title = delete_junk_symbol(cur_title);
                cur_title = low_letters_only(cur_title);
                bool the_same = false;
                if (prep_title.find(cur_title) != std::string::npos) {
                    the_same = true;
                }
                if (the_same) {
                    result[i].set_precision(100);
                } else {
                    int lev_distance = levenshtein_distance(cur_title, prep_title);
                    result[i].set_precision(100-(int)(100*lev_distance/prep_title.size()));
                }
            }
            sort(result.begin(), result.end(), greater);

            //    if (result[0].get_precision() == 100) {
            //       result.resize(1);
            //    } else {
            //       result.resize(5);
            //    }
        }
    }
    return result;
}
