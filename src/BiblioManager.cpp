#include <algorithm>
#include <ctime>
#include <iomanip>
#include <thread>
#include <queue>

#include "BiblioManager.h"
#include "RequesterManager.h"


using namespace std;

mutex m_in;
mutex m_out;

std::vector<ArticleInfo> BiblioManager::search_requester(Requester &requester, std::string query) {
    vector<ArticleInfo> result = {};
    vector<ArticleInfo> additional_result = {};

    transform(query.begin(), query.end(), query.begin(), ::tolower);
    string new_query = query;

    result = requester.publication_request(new_query);

/*    replace(new_query.begin(), new_query.end(), ' ', '.');
    additional_result = requester.publication_request(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    replace(query.begin(), query.end(), ' ', '$');
    additional_result = requester.publication_request(query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());*/

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

void BiblioManager::print_txt(std::ostream &out, const std::string &filename, ArticleInfo &result) {
    out << "--------------------------------------------------------------" << endl;
    out << filename << endl;
    out << "--------------------------------------------------------------" << endl;
    out << result.to_string() << " \n";
}

void BiblioManager::print_bib(std::ostream &out, vector<ArticleInfo> &result) {
    size_t result_size = result.size();
    for (size_t k = 0; k < result_size; k++) {
        if (result[k].get_authors().size() > 0) {
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
}


void BiblioManager::print_html(std::ostream &out, std::vector<ArticleInfo> &result) {

    size_t result_size = result.size();
    out << "<html>\n";
    out << "\t<head>\n";
    out << "\t\t<title>Biblio results</title>\n";
    out << "\t</head>\n";
    out << "\t<body>\n";
    for (size_t i = 0; i < result_size; i++) {
        out << "--------------------------------------------------------------" << endl;
        out << "<p><a href=\"" << result[i].get_filename() << "\">" << result[i].get_filename() << "</a></p>" << endl;
        out << "--------------------------------------------------------------" << endl;
        out << "<br>\n";
        out << "<pre>\n";

        vector<string> authors = result[i].get_authors();
        size_t t = authors.size();
        if (t > 0) {
            for (size_t j = 0; j < t - 1; ++j) {
                out << authors[j] << ", ";
            }
            out << authors[t - 1] << ":\n";
        }
        out << result[i].get_title();
        if (result[i].get_type() != "") {
            out << " " << result[i].get_type() << ".";
        }
        if (result[i].get_venue() != "") {
            out << " " << result[i].get_venue() << ".";
        }
        if (result[i].get_volume() != "") {
            out << " " << result[i].get_volume() << ".";
        }
        if (result[i].get_year() != "") {
            out << " " << result[i].get_year() << ".";
        }
        if (result[i].get_pages() != "") {
            out << " " << result[i].get_pages() << ".";
        }
        if (result[i].get_number() != "") {
            out << " " << result[i].get_number() << ".";
        }
        if (result[i].get_url() != "") {
            out << "\n" << result[i].get_url() << endl;
        }
        out << "</pre>\n";
    }
    out << "</pre>\n";
    out << "\t</body>\n";
    out << "</html>\n";
}

BiblioManager::BiblioManager(int threads) {
    this->threads_num = threads;
}

bool BiblioManager::my_empty(std::queue<std::string> &in) {
    m_in.lock();
    bool empty = in.empty();
    m_in.unlock();
    return empty;
}

std::string BiblioManager::my_pop(std::queue<std::string> &in) {
    m_in.lock();
    string name;
    name = in.front();
    in.pop();
    m_in.unlock();
    return name;
}

void BiblioManager::my_push(ArticleInfo info, std::vector<ArticleInfo> &out) {
    m_out.lock();
    out.push_back(info);
    m_out.unlock();
}

std::vector<ArticleInfo>
BiblioManager::search_distance(std::function<size_t(const std::string &,
                                                    const std::string &)> dist,
                               const std::vector<std::string> &filenames, bool offline) {
    queue<string, deque<string>> in(deque<string>(filenames.begin(), filenames.end()));
    vector<ArticleInfo> out = {};
    vector<thread> threads;

    for(int i = 0; i < threads_num; ++i){
        threads.push_back(std::thread(thread_function, dist, offline, ref(in), ref(out)));
    }

    for(auto& thread : threads){
        thread.join();

    }

    return out;
}

void BiblioManager::thread_function(std::function<size_t(const std::string &, const std::string &)> dist, bool offline,
                                    std::queue<std::string> &in, std::vector<ArticleInfo> &out) {
    string filename, picture_name;
    regex re_name("[^0-9]");
    vector<ArticleInfo> result = {};
    RequesterManager req_manager = RequesterManager();
    vector<Requester *> requesters = req_manager.get_all_requesters();
    bool found = false;
    while(!my_empty(in)) {
        filename = my_pop(in);
        picture_name = regex_replace(filename, re_name, "");
        PictureParser picture_parser = PictureParser(filename, 300, 300, "test_" + picture_name + ".png", "png", 700);
        picture_parser.find_title();
        string saved_title = picture_parser.get_title();
        string title = low_letters_only(saved_title);
        if (offline) {
            my_push(ArticleInfo(saved_title, filename), out);
            continue;
        }
        found = false;
        for (size_t k = 0; k < requesters.size(); k++) {
            result = search_requester(*requesters[k], saved_title);
            size_t result_size = result.size();
            if (result_size > 0) {
                for (size_t i = 0; i < result_size; i++) {
                    string cur_title = low_letters_only(result[i].get_title());
                    size_t distance = dist(cur_title, title);
                    int precision = 100 - (int) (100 * distance / max(title.size(), cur_title.size()));
                    result[i].set_precision(precision);
                }
                stable_sort(result.begin(), result.end(), greater);
                if (result[0].get_precision() > 90) {
                    result[0].set_filename(filename);
                    my_push(result[0], out);
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            my_push(ArticleInfo(saved_title, filename), out);
        }
    }
}

BiblioManager::BiblioManager() {
    threads_num = 1;
}
