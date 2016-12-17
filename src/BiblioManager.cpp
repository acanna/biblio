#include <algorithm>
#include <ctime>
#include <iomanip>
#include <thread>
#include <queue>

#include "BiblioManager.h"
#include "RequesterManager.h"
#include "BiblioThreadContext.h"


using namespace std;

std::vector<ArticleInfo> BiblioManager::search_requester(Requester &requester, std::string query) {
    transform(query.begin(), query.end(), query.begin(), ::tolower);
	vector<ArticleInfo> result = {};
    vector<ArticleInfo> additional_result = {};
	result = requester.publication_request(query);
    vector<string> words = split(query, ' ');

	// discard first word
	string new_query = "";
	for (unsigned int i = 1; i < words.size()-1; i++) {
	 new_query += words[i] + " ";
	}
	new_query += words[words.size()-1];
	additional_result = requester.publication_request(new_query);
	result.insert(result.end(), additional_result.begin(), additional_result.end());
	
	// discard last word
	new_query = "";
	for (unsigned int i = 0; i < words.size()-2; i++) {
	 new_query += words[i] + " ";
	}
	new_query += words[words.size()-2];
	additional_result = requester.publication_request(new_query);
	result.insert(result.end(), additional_result.begin(), additional_result.end());
	
	// discard first two words
	new_query = "";
	for (unsigned int i = 2; i < words.size()-1; i++) {
	 new_query += words[i] + " ";
	}
	new_query += words[words.size()-1];
	additional_result = requester.publication_request(new_query);
	result.insert(result.end(), additional_result.begin(), additional_result.end());
	
	// discard last two words
	new_query = "";
	for (unsigned int i = 0; i < words.size()-3; i++) {
		new_query += words[i] + " ";
	}
	new_query += words[words.size()-3];
	additional_result = requester.publication_request(new_query);
	result.insert(result.end(), additional_result.begin(), additional_result.end());
	
	if (words.size() > 10) {
		// discard first five words
		new_query = "";
		for (unsigned int i = 5; i < words.size()-1; i++) {
		 new_query += words[i] + " ";
		}
		new_query += words[words.size()-1];
		additional_result = requester.publication_request(new_query);
		result.insert(result.end(), additional_result.begin(), additional_result.end());
		
		// discard last five words
		new_query = "";
		for (unsigned int i = 0; i < words.size()-6; i++) {
		 new_query += words[i] + " ";
		}
		new_query += words[words.size()-6];
		additional_result = requester.publication_request(new_query);
		result.insert(result.end(), additional_result.begin(), additional_result.end());
     }
     return result;
 }


bool BiblioManager::greater(const ArticleInfo &info_1, const ArticleInfo &info_2) {
    return (info_1.get_precision() > info_2.get_precision());
}

void BiblioManager::print_bib(std::ostream &out, vector<ArticleInfo> &result) {
    for (size_t k = 0; k < result.size(); k++) {
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
        } else {
            out << "@ARTICLE{" << result[k].get_filename() << ",\n";
            out << "title = {" << result[k].get_title() << "}";
            out << "\n}\n\n";
        }
    }
}

void BiblioManager::start_print_html(std::ostream &out) {
    out << "<!DOCTYPE html>\n";
    out << "<html>\n";
    out << "\t<head>\n";
    out << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
    out << "\t\t<title>Biblio results</title>\n";
    out << "\t</head>\n";
    out << "\t<body>\n";
    out << "\t\t<table border=\"1\" width=\"100%\" cellpadding=\"5\" bgcolor=\"#42D6FF\">\n";
}

void BiblioManager::end_print_html(std::ostream &out) {
    out << "\t\t</table>" << endl;
    out << "\t</body>\n";
    out << "</html>\n";
}

void BiblioManager::print_html(std::ostream &out, std::vector<ArticleInfo> &result) {
    for (size_t i = 0; i < result.size(); i++) {
        out << "\t\t\t<tr>\n";
        out << "\t\t\t\t<td align=\"center\"><a href=\"" << result[i].get_filename() << "\">" << result[i].get_filename() << "</a></td>\n";
        if (i % 2 == 0) {
            out << "\t\t\t\t<td bgcolor=\"#FFE780\">\n";
        } else {
            out << "\t\t\t\t<td bgcolor=\"#FBF9CB\">\n";
        }
        out << "\t\t\t\t\t<pre>  ";
        vector<string> authors = result[i].get_authors();
        size_t t = authors.size();
        if (t > 0) {
            for (size_t j = 0; j < t - 1; ++j) {
                out << authors[j] << ", ";
            }
            out << authors[t - 1] << ":\n  ";
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
            out << "\n  " << "<a href=\"" << result[i].get_url() << "\">" << result[i].get_url() << "</a>";
        }
        out << "</pre>\n";
        out << "\t\t\t\t</td>\n";
        out << "\t\t\t</tr>\n";
    }
}

BiblioManager::BiblioManager(int threads) {
    this->threads_num = threads;
}

std::vector<ArticleInfo>
BiblioManager::search_distance(std::function<size_t(const std::string &,
                                                    const std::string &)> dist, bool offline) {
    vector<thread> threads;
    for(int i = 0; i < threads_num; ++i){
        threads.push_back(std::thread(thread_function, dist, offline));
    }

    for(auto& thread : threads){
        thread.join();
    }
    return BiblioThreadContext::instance().get_output();
}

void
BiblioManager::thread_function(std::function<size_t(const std::string &, const std::string &)> dist, bool offline) {
    string filename;
    vector<ArticleInfo> result = {};
    RequesterManager req_manager = RequesterManager();
    vector<Requester *> requesters = req_manager.get_all_requesters();
    bool found = false;
    while(!BiblioThreadContext::instance().my_empty()) {
        filename = BiblioThreadContext::instance().my_pop();
		if (filename == "") {
			break;
		}

        PictureParser picture_parser = PictureParser(filename, 300, 300, get_random_filename() + ".png", "png", 700);
        string saved_title = raw_to_formatted(picture_parser.find_title());
        string title = low_letters_only(saved_title);
        if (offline || title.size() == 0) {
            BiblioThreadContext::instance().my_push(ArticleInfo(saved_title, filename));
            continue;
        }
        found = false;
        for (size_t k = 0; k < requesters.size(); k++) {
            result = search_requester(*requesters[k], saved_title);
            if (result.size() > 0) {
                for (size_t i = 0; i < result.size(); i++) {
                    string cur_title = raw_to_formatted(result[i].get_title());
                    cur_title = low_letters_only(cur_title);
                    size_t distance = dist(cur_title, title);
                    int precision = 100 - (int) (100 * distance / max(title.size(), cur_title.size()));
                    result[i].set_precision(precision);
                }
                stable_sort(result.begin(), result.end(), greater);
                if (result[0].get_precision() > 90) {
                    result[0].set_filename(filename);
                    BiblioThreadContext::instance().my_push(result[0]);
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            BiblioThreadContext::instance().my_push(ArticleInfo(saved_title, filename));
        }
    }
}

BiblioManager::BiblioManager() {
    threads_num = 1;
}

void BiblioManager::cout_not_found_articles(std::vector<ArticleInfo> &result) {
    cout << "=========================================================================" << endl;
    cout << "                       Start not found articles                          " << endl;
    cout << "=========================================================================" << endl << endl;
    size_t result_size = result.size(), found = result_size;
    for (size_t k = 0; k < result_size; k++) {
        if (result[k].get_authors().size() == 0) {
            found--;
            cout << "filename: " << result[k].get_filename() << endl;
            cout << "title: " << result[k].get_title() << endl << endl;
        }
    }
    cout << "Found information about " << found << " articles from " << result_size << endl;
    cout << "=========================================================================" << endl;
    cout << "                         End not found articles                          " << endl;
    cout << "=========================================================================" << endl;
}

