#include <algorithm>
#include <ctime>
#include <iomanip>
#include <thread>
#include "BiblioManager.h"


using namespace std;

vector<ArticleInfo> BiblioManager::search_dblp(string query) {
    vector<ArticleInfo> result = {};
    vector<ArticleInfo> additional_result = {};
    DBLPManager requester_my;
    transform(query.begin(), query.end(), query.begin(), ::tolower);
    string new_query = query;

    result = requester_my.publicationRequest(new_query);

    replace(new_query.begin(), new_query.end(), ' ', '.');
    additional_result = requester_my.publicationRequest(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    replace(query.begin(), query.end(), ' ', '$');
    additional_result = requester_my.publicationRequest(query);
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

BiblioManager::BiblioManager() {}

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

        out << "@ARTICLE{" << label << ",\n";
        out << "author = {";
        for (size_t i = 0; i < t - 1; ++i) {
            out << authors[i] << " and\n";
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
    out << "<p><a href=\"" << filename << "\">" << filename << "</a></p>" << endl;
    out << "--------------------------------------------------------------" << endl;
    out << "<br>\n";
    out << "<pre>\n";

    if (result_size == 0) {
        out << "<p>NOT FOUND</p>" << endl;
    }

    for (size_t k = 0; k < result_size; ++k) {
        vector<string> authors = result[k].get_authors();
        size_t t = authors.size();
        if (t > 0) {
            for (size_t i = 0; i < t - 1; ++i) {
                out << authors[i] << ", ";
            }
            out << authors[t - 1] << ":\n";
        }
        out << result[k].get_title();
        if (result[k].get_type() != "") {
            out << " " << result[k].get_type() << ".";
        }
        if (result[k].get_venue() != "") {
            out << " " << result[k].get_venue() << ".";
        }
        if (result[k].get_volume() != "") {
            out << " " << result[k].get_volume() << ".";
        }
        if (result[k].get_year() != "") {
            out << " " << result[k].get_year() << ".";
        }
        if (result[k].get_pages() != "") {
            out << " " << result[k].get_pages() << ".";
        }
        if (result[k].get_number() != "") {
            out << " " << result[k].get_number() << ".";
        }
        if (result[k].get_url() != "") {
            out << "\n" << result[k].get_url() << endl;
        }
    }
    out << "</pre>\n";
    out << "\t</body>\n";
    out << "</html>\n";
}

vector<ArticleInfo> BiblioManager::search_levenshtein(const string &filename, bool offline) {

    vector<string> title_candidates;

    vector<ArticleInfo> result = {};
    vector<ArticleInfo> dblp_result = {};

    try {
        parser = Parser(filename);
        title_candidates = parser.get_title();
        size_t n = title_candidates.size();
        for (size_t i = 0; i < n - 1; ++i) {
            title_candidates.push_back(title_candidates[i] + " " + title_candidates[i + 1]);
        }
    } catch (const Biblio_exception &e) {
        throw;
    }

    if (!offline) {

        try {
            for (string &s : title_candidates) {
                dblp_result = search_dblp(s);

                if (dblp_result.size() > 0) {
                    string ss = low_letters_only(s);
                    size_t result_size = dblp_result.size();
                    for (size_t i = 0; i < result_size; i++) {
                        string cur_title = dblp_result[i].get_title();
                        cur_title = low_letters_only(cur_title);

                        size_t lev_distance = levenshtein_distance(cur_title, ss);
                        dblp_result[i].set_precision(
                                100 - (int) (100 * lev_distance / max(ss.size(), cur_title.size())));
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
        catch (const Biblio_exception &e) {
            throw;
        }
    }
    if (result.size() > 0) {
        vector<ArticleInfo> res = {};
        if (result[0].get_precision() == 100) {
            res.push_back(result[0]);
        }
        return res;
    }
    return result;
}


vector<ArticleInfo> BiblioManager::search_levenshtein_light(const string &filename, bool offline) {

    vector<string> title_candidates;
    vector<ArticleInfo> result = {};
    vector<ArticleInfo> dblp_result = {};

    try {
        parser = Parser(filename);
        title_candidates = parser.get_title();
        size_t n = title_candidates.size();
        for (size_t i = 0; i < n - 1; ++i) {
            title_candidates.push_back(title_candidates[i] + " " + title_candidates[i + 1]);
        }
    } catch (const Biblio_exception &e) {
        throw;
    }

    if (!offline) {
        try {
            for (string &s : title_candidates) {
                transform(s.begin(), s.end(), s.begin(), ::tolower);
                dblp_result = search_dblp(s);
                if (dblp_result.size() > 0) {

                    size_t result_size = dblp_result.size();
                    for (size_t i = 0; i < result_size; i++) {
                        string cur_title = dblp_result[i].get_title();
                        transform(cur_title.begin(), cur_title.end(), cur_title.begin(), ::tolower);
                        size_t lev_distance = levenshtein_distance(cur_title, s);
                        dblp_result[i].set_precision(
                                100 - (int) (100 * lev_distance / max(s.size(), cur_title.size())));
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
        catch (const Biblio_exception &e) {
            throw;
        }
    }
    if (result.size() > 0) {
        vector<ArticleInfo> final_result;
        final_result.push_back(result[0]);
        return final_result;
    }
    return result;
}


vector<ArticleInfo> BiblioManager::search_exact_match(const string &filename, bool offline) {

    vector<string> title_candidates = {};
    vector<ArticleInfo> result = {};
    vector<ArticleInfo> dblp_result = {};

    try {
        parser = Parser(filename);
        title_candidates = parser.get_title();
        size_t n = title_candidates.size();
        for (size_t i = 0; i < n - 1; ++i) {
            title_candidates.push_back(title_candidates[i] + " " + title_candidates[i + 1]);
        }
    } catch (const Biblio_exception &e) {
        throw;
    }

    string title = "";
    for (string s : title_candidates) {
        title += s + " ";
    }

    if (!offline) {
        try {
            string prep_title = low_letters_only(title);

            for (string &s : title_candidates) {

                dblp_result = search_dblp(s);

                if (dblp_result.size() > 0) {
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
        catch (const Biblio_exception &e) {
            throw;
        }
    }
    vector<ArticleInfo> res = {};
    if (result.size() > 0) {
        res.push_back(result[0]);
    }
    return res;
}

BiblioManager::BiblioManager(std::string &filename) {
    //parser = Parser(filename);
    picture_parser = PictureParser(filename, 300, 300, "test.png", "png", 700);
}

void BiblioManager::thread_search_function(int i, vector<string> &title_candidates, std::vector<std::vector<ArticleInfo>> &results) {
    try {
        vector<ArticleInfo> dblp_result = {};
        size_t iters = (title_candidates.size() - 1 - i) / 4;
        for (size_t j = 0; j < iters; j++) {
            string query = title_candidates[j * 4 + i];
            dblp_result = search_dblp(query);
            size_t result_size = dblp_result.size();
            if (result_size > 0) {
                for (size_t k = 0; k < result_size; k++) {
                    string cur_title = dblp_result[k].get_title();
                    size_t lev_distance = levenshtein_distance(cur_title, query);
                    dblp_result[k].set_precision(
                            100 - (int) (100 * lev_distance / max(query.size(), cur_title.size())));
                }
                results[i].insert(results[i].end(), dblp_result.begin(), dblp_result.end());
            }
        }
    } catch (...) {}
}

std::vector<ArticleInfo> BiblioManager::search_levenshtein_light_threads(const std::string &filename, bool offline) {
    vector<string> title_candidates;
    vector<ArticleInfo> result = {};
    vector<vector<ArticleInfo>> results(4);
    vector<thread> threads = {};
    try {
        parser = Parser(filename);
        title_candidates = parser.get_title();
        size_t n = title_candidates.size();
        for (size_t i = 0; i < n - 1; ++i) {
            title_candidates.push_back(title_candidates[i] + " " + title_candidates[i + 1]);
        }
    } catch (const Biblio_exception &e) {
        throw;
    }
    if (!offline) {
        try {
            for (int i = 0; i < 4; i++) {
                threads.push_back(thread(thread_search_function, i, ref(title_candidates), ref(results)));
                threads[i].join();
            }
            for (int i = 0; i < 4; i++) {
                result.insert(result.end(), results[i].begin(), results[i].end());
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
        catch (const Biblio_exception &e) {
            throw;
        }
    }
    if (result.size() > 0) {
        vector<ArticleInfo> final_result;
        final_result.push_back(result[0]);
        return final_result;
    }
    return result;
}

std::vector<ArticleInfo> BiblioManager::search_title(const std::string &title, std::ostream &out) {
    vector<ArticleInfo> result = BiblioManager::search_dblp(title);
    size_t result_size = result.size();
    if (result_size > 0) {

        string ss = low_letters_only(title);
        for (size_t i = 0; i < result_size; i++) {
            string cur_title = low_letters_only(result[i].get_title());

            size_t lev_distance = levenshtein_distance(cur_title, ss);
            result[i].set_precision(
                    100 - (int) (100 * lev_distance / max(ss.size(), cur_title.size())));
        }
        stable_sort(result.begin(), result.end(), greater);
        int t = result[0].get_precision();
        size_t i = 0;
        while (i < result.size() && result[i].get_precision() == t) i++;
        i--;
        stable_sort(result.begin(), result.begin() + i, longer_title);
        print_html(out, title, result);
        vector<ArticleInfo> final_result;
        final_result.push_back(result[0]);
        return final_result;
    }
    return result;
}

std::vector<ArticleInfo>
BiblioManager::search_with_distance(std::function<size_t(const std::string &, const std::string &)> dist,
                                    const std::string &filename, bool offline) {
    picture_parser = PictureParser(filename, 300, 300, "test.png", "png", 700);
    picture_parser.find_title();
    vector<ArticleInfo> result = {};
    string title = picture_parser.get_title();
    if (offline) {
        result.push_back(ArticleInfo(title));
        return result;
    }
    result = BiblioManager::search_dblp(title);
    size_t result_size = result.size();
    vector<ArticleInfo> final_result = {};
    string saved_title = title;
    if (result_size > 0) {
        title = low_letters_only(title);
        for (size_t i = 0; i < result_size; i++) {
            string cur_title = low_letters_only(result[i].get_title());
            size_t distance = dist(cur_title, title);
            int precision = 100 - (int) (100 * distance / max(title.size(), cur_title.size()));
            result[i].set_precision(precision);
        }
        stable_sort(result.begin(), result.end(), greater);

//        int t = result[0].get_precision();
//        size_t i = 0;
//        while (i < result.size() && result[i].get_precision() == t) i++;
//        i--;
//        stable_sort(result.begin(), result.begin() + i, longer_title);

        // магическая константа 90 упала с неба и требует её удаления или замены
        if (result[0].get_precision() > 90) {
            final_result.push_back(result[0]);
        } else {
            final_result.push_back(ArticleInfo(title));
        }
    } else {
        final_result.push_back(ArticleInfo(saved_title));
    }
    return final_result;
}
