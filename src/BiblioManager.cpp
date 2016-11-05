#include <algorithm>
#include "BiblioManager.h"
#include "tools.h"


using namespace std;

std::vector<ArticleInfo> BiblioManager::search_damerau_levenshtein(const std::string &filename, bool offline) {

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

                    string ss = low_letters_only(s);

                    size_t result_size = dblp_result.size();

                    for (size_t i = 0; i < result_size; i++) {

                        string cur_title = dblp_result[i].get_title();
                        cur_title = low_letters_only(cur_title);

                        size_t dist = damerauLevenshteinDistance(ss, cur_title);
                        dblp_result[i].set_precision(100 - (int) (100 * dist / max(ss.size(), cur_title.size())));

                    }
                    result.insert(result.end(), dblp_result.begin(), dblp_result.end());
                }
                if (result.size() > 30) {
                    break;
                }
            }
            if (result.size() > 0) {
                stable_sort(result.begin(), result.end(), greater);
                int t = result[0].get_precision();
                size_t i = 0;
                while (i < result.size() && result[i].get_precision() == t) i++;
                i--;
                stable_sort(result.begin(), result.begin() + i, longer_title);
            }
        }
        catch (const Biblio_exception & e) {
            throw;
        }
    }
    return result;
}

vector<ArticleInfo> BiblioManager::search_dblp(DBLPManager & dblp, string & query) {

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

bool BiblioManager::smaller(const ArticleInfo &info_1, const ArticleInfo &info_2) {
    return info_1.get_precision() <= info_2.get_precision();
}

bool BiblioManager::longer_title(const ArticleInfo &info_1, const ArticleInfo &info_2) {
    return info_1.get_title().size() > info_2.get_title().size();
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
        out << result[k].to_string();
    }
    out << "</pre>\n";
    out << "\t</body>\n";
    out << "</html>\n";
}

vector<ArticleInfo> BiblioManager::search_levenshtein(const string & filename, bool offline) {

    list<string> auths_candidates;
    vector<string> title_candidates;

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
                    string ss = low_letters_only(s);
                    size_t result_size = dblp_result.size();
                    for (size_t i = 0; i < result_size; i++) {
                        string cur_title = dblp_result[i].get_title();
                        cur_title = low_letters_only(cur_title);

                        size_t lev_distance = levenshtein_distance(cur_title, ss);
                        dblp_result[i].set_precision(100 - (int) (100 * lev_distance / max(ss.size(), cur_title.size())));

                    }
                    result.insert(result.end(), dblp_result.begin(), dblp_result.end());
                }
            }
            if (result.size() > 0) {
                stable_sort(result.begin(), result.end(), greater);
                int t = result[0].get_precision();
                size_t i = 0;
                while (i < result.size() && result[i].get_precision() == t) i++;
                i--;
                stable_sort(result.begin(), result.begin() + i, longer_title);
            }
        }
        catch (const Biblio_exception & e) {
            throw;
        }
    }
    return result;
}

vector<ArticleInfo> BiblioManager::search_exact_match(const string & filename, bool offline) {

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

                    string ss = low_letters_only(s);

                    size_t result_size = dblp_result.size();

                    for (size_t i = 0; i < result_size; i++) {

                        string cur_title = dblp_result[i].get_title();
                        cur_title = low_letters_only(cur_title);

                        if (prep_title.find(cur_title) != string::npos) {

                            dblp_result[i].set_precision(100);
                            result.push_back(dblp_result[i]);
                        }
                    }
                }
            }
            sort(result.begin(), result.end(), longer_title);
        }
        catch (const Biblio_exception & e) {
            throw;
        }
    }
    vector<ArticleInfo> res = {};
    if (result.size() > 0) {
        res.push_back(result[0]);
    }
    return res;
}
