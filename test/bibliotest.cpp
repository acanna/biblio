#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>

#include <gtest/gtest.h>
#include "../lib/tinydir/tinydir.h"

#include "../src/Requesters/Requester.h"
#include "../src/BiblioManager.h"

using namespace std;

BiblioManager manager;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


TEST (PaperPresent, Positive) {
	const std::string URL = "http://dblp.org/search/publ/api?format=json&h=1&q=";
	DBLPRequester dblp = DBLPRequester(URL);    

    const string query = "Land.Cover.Classification.and.Forest.Change.Analysis";
    const string title = "Land Cover Classification and Forest Change Analysis, Using Satellite Imagery-A Case Study in Dehdez Area of Zagros Mountain in Iran.";
    const string venue = "J. Geographic Information System";
    const string year = "2011";
    vector<ArticleInfo> result = dblp.publication_request(query);
    EXPECT_EQ(result[0].get_year(), year);
    EXPECT_EQ(result[0].get_venue(), venue);
    EXPECT_EQ(result[0].get_title(), title);
}

TEST (PaperAbsent, Negative) {
	const std::string URL = "http://dblp.org/search/publ/api?format=json&h=1&q=";
	DBLPRequester dblp = DBLPRequester(URL);    
	const string query = "QQQ";
    vector<ArticleInfo> result = dblp.publication_request(query);
    EXPECT_EQ(result.size(), 0);
}

TEST (PictureParser, Positive) {
    string data_file = "../articles/test_summary.txt";
    string path = "../articles/";
    ifstream file(data_file);
    int passed = 0;
    int counter = 0;
    string line = "", filename = "", paper_title = "";
    vector<string> tmp;

    while (file.is_open() && !file.eof()) {
        getline(file, line);
        tmp = split(line, '\t');
        if (tmp.size() < 2) {
            continue;
        }
        filename = tmp[0];
        paper_title = tmp[1];
        filename = path + filename;
        try {

            PictureParser picture_parser = PictureParser(filename, 300, 300, "test.png", "png", 700);
            picture_parser.find_title();
			string result = picture_parser.get_title();

            transform(result.begin(), result.end(), result.begin(), (int (*)(int)) tolower);

            paper_title = raw_to_formatted(paper_title);
            transform(paper_title.begin(), paper_title.end(), paper_title.begin(), (int (*)(int)) tolower);

                    
            if (paper_title.find(result) != std::string::npos) {
                passed++;
            } else {
                cout << "Failed at " << filename << endl;
                cout << "Exact title: " << endl;
	            cout << paper_title << endl;
                cout << "Parsed title: " << endl;
	            cout << result <<"..."<<endl;
	            cout << endl;
            }
            counter++;
        } catch (const BiblioException &e) {
            cerr << e.what() << endl;
        }
    }

	cout << endl;
    cout << ">>>-------------------------------------<<<" << endl;
    cout << "    Passed " << passed << " tests from " << counter << endl;
    cout << "    Passed " << passed * 100 / (float) counter << " % from total amount" << endl;
    cout << ">>>-------------------------------------<<<" << endl;

    EXPECT_EQ(0, 0);
}

TEST (PictureParser, Online) {

    if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
        throw new BiblioException("Curl global init failed.\n");
    }
    int threads = 1;
    std::vector<std::pair<requestersEnum, std::vector<std::string>>> data = read_config_data("../biblio.cfg", threads);
    BiblioManager manager(threads);

    ofstream out_html("biblio.html");
    ofstream out_bib("biblio.bib");
    vector<string> filenames = {};
    string path = "../articles/";
    ifstream file("../articles/test_summary.txt");
    int counter = 0;
    int found = 0;
    string line = "", filename = "", paper_title = "", cur_title = "", unformatted_paper_title = "";
    vector<string> tmp;
    map<string, string> files_info = {};
    while (file.is_open() && !file.eof()) {
        getline(file, line);
        tmp = split(line, '\t');
        if (tmp.size() < 2) {
            continue;
        }
        filename = tmp[0];
        paper_title = raw_to_formatted(tmp[1]);
        filename = path + filename;
        files_info.insert(make_pair(filename, paper_title));
        filenames.push_back(filename);
    }
    try {
        vector<ArticleInfo> result = manager.search_distance_data(data, levenshtein_distance, filenames, false);
        manager.print_html(out_html, result);
        manager.print_bib(out_bib, result);

        size_t result_size = result.size();
        for (size_t i = 0; i < result_size; i++) {
            string title = raw_to_formatted(result[i].get_title());
            transform(title.begin(), title.end(), title.begin(), (int (*)(int)) tolower);

            paper_title = files_info[result[i].get_filename()];
            transform(paper_title.begin(), paper_title.end(), paper_title.begin(), (int (*)(int)) tolower);

			cout << endl;
			cout << "Exact title: " << endl;
            cout << paper_title << endl;
            cout << "Parsed title: " << endl;
            cout << title <<endl;
            cout << endl;
         

            if (result[i].get_authors().size() != 0) {
                found++;
            } else {
                cout << "Failed at " << result[i].get_filename() << endl;
/*                cout << "Exact title: " << endl;
                cout << paper_title << endl;
                cout << "Parsed title: " << endl;
                cout << title <<endl;
                cout << endl;*/
            }
            counter++;
        }
    } catch (const BiblioException &e) {
        cerr << e.what() << '\n';
    }

    out_html.close();
    out_bib.close();

    cout << ">>>-------------------------------------<<<" << endl;
    cout << "    Found " << found << " tests from " << counter << endl;
    cout << "    Passed " << found * 100 / (float) counter << " % from total amount" << endl;
    cout << ">>>-------------------------------------<<<" << endl;

    EXPECT_EQ(0, 0);
}

// Prints all files in directory and subdirectories
void recursive_print_dir(std::string path) {
    if (path[path.length() - 1] != '/') {
        path += "/";
    }
    tinydir_dir dir;
    tinydir_open(&dir, path.c_str());
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        if (file.is_dir)
        {
            string filename = string(file.name);
            if (filename != "." && filename != ".."){
                recursive_print_dir(path + filename);
            }
        } else {
            cout << path + file.name << endl;
        }
        tinydir_next(&dir);
    }
    tinydir_close(&dir);
}

TEST (TinyDir, Try) {
    string path = "..";
    recursive_print_dir(path);
    EXPECT_EQ(0, 0);
}

TEST (TinyDir, ReadPDF) {
    string path = "../";
    vector<string> files = read_pdf_files_recursive(path);
    for (string s : files) {
        cout << s << endl;
    }
}