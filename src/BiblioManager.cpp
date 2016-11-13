#include <algorithm>
#include <ctime>
#include <iomanip>
#include <thread>
#include "BiblioManager.h"
#include "tools.h"


using namespace std;

std::vector<ArticleInfo> BiblioManager::search_damerau_levenshtein(const std::string &filename, bool offline) {

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
        string prep_title = low_letters_only(title);
        for (string &s : title_candidates) {

            dblp_result = search_dblp(s);
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
    return result;
}

vector<ArticleInfo> BiblioManager::search_dblp(string query) {

    vector<ArticleInfo> result = {};
    vector<ArticleInfo> additional_result = {};

    DBLPManager requester_my;

    query = delete_multiple_spaces(query);

    transform(query.begin(), query.end(), query.begin(), ::tolower);
    string new_query = query;
    string one_more_query = query;
    result = requester_my.publicationRequest(new_query);

    replace(new_query.begin(), new_query.end(), ' ', '.');
    additional_result = requester_my.publicationRequest(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    replace(one_more_query.begin(), one_more_query.end(), ' ', '$');
    additional_result = requester_my.publicationRequest(one_more_query);
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
        for (size_t i = 0; i < t - 1; ++i) {
            out << authors[i] << ", ";
        }
        out << authors[t - 1] << ":\n";
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

vector<ArticleInfo> BiblioManager::search_levenshtein(ostream &out, const string &filename, bool offline) {
    out << "-----------------------New_Article----------------------" << endl;
    std::clock_t c_start = std::clock();
    list <string> auths_candidates;
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

    string title = "";

    for (string s : title_candidates) {
        title += s + " ";
    }

    if (!offline) {
        clock_t search_sum = 0, processing_sum = 0;
        int search = 0;
        try {
            string prep_title = low_letters_only(title);
            for (string &s : title_candidates) {

                clock_t req_start = clock();
                dblp_result = search_dblp(s);
                clock_t req_end = clock();
                out << std::fixed << std::setprecision(2) << "CPU time used for search: "
                    << 1000.0 * (req_end - req_start) / CLOCKS_PER_SEC << " ms\n";
                search_sum += (1000.0 * (req_end - req_start) / CLOCKS_PER_SEC);
                search++;
                out << "dblp_result_size: " << dblp_result.size() << endl;
                if (dblp_result.size() > 0) {

                    clock_t beg = clock();

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

                    clock_t end = clock();
                    out << std::fixed << std::setprecision(2) << "CPU time used for processing: "
                        << 1000.0 * (end - beg) / CLOCKS_PER_SEC << " ms\n";
                    processing_sum += (1000.0 * (end - beg) / CLOCKS_PER_SEC);
                }
            }
            clock_t beg = clock();

            if (result.size() > 0) {
                stable_sort(result.begin(), result.end(), greater);
                int t = result[0].get_precision();
                size_t i = 0;
                while (i < result.size() && result[i].get_precision() == t) i++;
                i--;
                stable_sort(result.begin(), result.begin() + i, longer_title);
            }

            clock_t end = clock();
            processing_sum += (1000.0 * (end - beg) / CLOCKS_PER_SEC);
        }
        catch (const Biblio_exception &e) {
            throw;
        }

        out << "================================================" << endl;
        out << "Average search time: " << search_sum / search << " ms" << endl;
        out << "Average processing time: " << processing_sum / search << " ms" << endl;
    }



    std::clock_t c_end = std::clock();
    out << std::fixed << std::setprecision(2) << "CPU time used: "
        << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << " ms\n";
    out << "-----------------------End----------------------" << endl;
    return result;
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

std::vector<std::string> BiblioManager::get_fst_page() {
    return parser.get_fst_page();
}

BiblioManager::BiblioManager(std::string &filename) {
    parser = Parser(filename);
}

void BiblioManager::thread_search_function(std::string s, std::vector<ArticleInfo> &result) {
    try {
        vector<ArticleInfo> dblp_result = {};
        dblp_result = search_dblp(s);
        size_t result_size = dblp_result.size();
        if (result_size > 0) {
            string ss = low_letters_only(s);
            for (size_t i = 0; i < result_size; i++) {
                string cur_title = low_letters_only(dblp_result[i].get_title());

                size_t lev_distance = levenshtein_distance(cur_title, ss);
                dblp_result[i].set_precision(
                        100 - (int) (100 * lev_distance / max(ss.size(), cur_title.size())));
            }
            result.insert(result.end(), dblp_result.begin(), dblp_result.end());
        }
    } catch (...) {}
}

std::vector<ArticleInfo> BiblioManager::search_levenshtein_light_threads(const std::string &filename, bool offline) {

    vector<string> title_candidates;
    vector<ArticleInfo> result = {};
    vector<ArticleInfo> dblp_result_1 = {};
    vector<ArticleInfo> dblp_result_2 = {};
    vector<ArticleInfo> dblp_result_3 = {};
    vector<ArticleInfo> dblp_result_4 = {};

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
            string str = title_candidates[title_candidates.size() - 1];
            size_t rest = 4 - (title_candidates.size() % 4);
            for (size_t i = 1; i <= rest; i++) {
                title_candidates.push_back(str);
            }
            size_t iterations = title_candidates.size() / 4;
            for (size_t i = 0; i < iterations; i++) {
                thread thread_1(thread_search_function, title_candidates[4 * i], ref(dblp_result_1));
                thread thread_2(thread_search_function, title_candidates[4 * i + 1], ref(dblp_result_2));
                thread thread_3(thread_search_function, title_candidates[4 * i + 2], ref(dblp_result_3));
                thread thread_4(thread_search_function, title_candidates[4 * i + 3], ref(dblp_result_4));
                thread_1.join();
                thread_2.join();
                thread_3.join();
                thread_4.join();
            }

            if (dblp_result_1.size() > 0) {
                result.insert(result.end(), dblp_result_1.begin(), dblp_result_1.end());
            }
            if (dblp_result_2.size() > 0) {
                result.insert(result.end(), dblp_result_2.begin(), dblp_result_2.end());
            }
            if (dblp_result_3.size() > 0) {
                result.insert(result.end(), dblp_result_3.begin(), dblp_result_3.end());
            }
            if (dblp_result_4.size() > 0) {
                result.insert(result.end(), dblp_result_4.begin(), dblp_result_4.end());
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
    vector <ArticleInfo> result = BiblioManager::search_dblp(title);
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
