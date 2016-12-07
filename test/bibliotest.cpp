#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <gtest/gtest.h>
#include "../src/DBLPRequester.h"
#include "../src/BiblioManager.h"
#include "../lib/tinydir/tinydir.h"

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

// Do we need this test? Using this template of manager.search_distance_requesters
// it does not work because of missing filename.
/*TEST (WrongQuery, Negative) {
    const string query = "http://dblp.org/search/pbl/api?q";
	DBLPRequester dblp = DBLPRequester(query);
	vector<Requester *> req = {};
	req.push_back(&dblp);
    vector<ArticleInfo> result = manager.search_distance_requesters(req, levenshtein_distance, filename, false);
    EXPECT_EQ(result.size(), 0);
}*/

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
        } catch (const Biblio_exception &e) {
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

TEST (PictureParser, OnlineDBLP) {
	const std::string URL = "http://dblp.org/search/publ/api?format=json&h=1&q=";
    string data_file = "../articles/test_summary.txt";
    string path = "../articles/";
    ifstream file(data_file);
    ofstream out_html("result.html");
    int passed = 0;
    int counter = 0;
    string line = "", filename = "", paper_title = "", cur_title = "";
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
        BiblioManager manager = BiblioManager();
        try {
			// dblp
            DBLPRequester *dblp = new DBLPRequester(URL);
			vector<Requester*> req = {};
			req.push_back(dblp);


            ArticleInfo result = manager.search_distance_requesters(req, levenshtein_distance, filename, false);
            manager.print_html(out_html, filename, result);
            paper_title = raw_to_formatted(paper_title);
            cur_title = raw_to_formatted(result.get_title());
            if (delete_spaces_to_lower(paper_title) == delete_spaces_to_lower(cur_title) ||
                paper_title.find(cur_title) != std::string::npos) {
                passed++;
            } else {
                cout << "Failed at " << filename << endl;
                cout << "Actual: " << paper_title << endl;
                cout << "Got:    " << result.get_title() << endl;
            }
            counter++;
        } catch (const Biblio_exception &e) {
            cerr << e.what() << endl;
        }
    }
    out_html.close();
    cout << ">>>-------------------------------------<<<" << endl;
    cout << "    Passed " << passed << " tests from " << counter << endl;
    cout << "    Passed " << passed * 100 / (float) counter << " % from total amount" << endl;
    cout << ">>>-------------------------------------<<<" << endl;

    EXPECT_EQ(0, 0);
}

//EL кажется, эти тесты по форме устарели. Конфиг не используется.

TEST (PictureParser, Online) {
	string dblp_url = "http://dblp.org/search/publ/api?format=json&h=1&q=";
	string springer_url = "http://api.springer.com/meta/v1/json?q=title:";
	string springer_apikey= "64f779d62e09f8ec669d4c656684cded";  
    string data_file = "../articles/test_summary.txt";
    string path = "../articles/";
    ifstream file(data_file);
    ofstream out_html("result.html");
    int counter = 0;
    int found_right = 0, found_wrong = 0;
    string line = "", filename = "", paper_title = "", cur_title = "", unformatted_paper_title = "";
    vector<string> tmp;
    vector<Requester *> req = {};
    // dblp
    Requester * dblp = new DBLPRequester(dblp_url);
    req.push_back(dblp);
    // springer
    Requester * springer = new SpringerRequester(springer_url, springer_apikey);
    req.push_back(springer);
    while (file.is_open() && !file.eof()) {
        getline(file, line);
        tmp = split(line, '\t');
        if (tmp.size() < 2) {
            continue;
        }
        filename = tmp[0];
        paper_title = tmp[1];
        unformatted_paper_title = paper_title;
        filename = path + filename;
        BiblioManager manager = BiblioManager();
        try {
            ArticleInfo result = manager.search_distance_requesters(req, levenshtein_distance, filename, false);

            manager.print_html(out_html, result);
            paper_title = letters_to_lower(paper_title);
            cur_title = letters_to_lower(result.get_title());
            if (result.get_authors().size() > 0) {
                if (paper_title.find(cur_title) != std::string::npos) {
                    found_right++;
                } else {
                    found_wrong++;
                    cout << "Found wrong. Failed at " << filename << endl;
                    cout << "Actual: " << unformatted_paper_title << endl;
                    cout << "Got:    " << result.get_title() << endl;
                }
            } else {
                cout << "Failed at " << filename << " (NOT FOUND)" << endl;
                cout << "Actual: " << unformatted_paper_title << endl;
                cout << "Got:    " <<result.get_title() << endl;
            }
            counter++;
        } catch (const Biblio_exception &e) {
            cerr << e.what() << endl;
        }
    }
    out_html.close();
    cout << ">>>-------------------------------------<<<" << endl;
    cout << "    Found right " << found_right << " tests from " << counter << endl;
    cout << "    Found wrong " << found_wrong << " tests from " << counter << endl;
    cout << "    Passed " << found_right * 100 / (float) counter << " % from total amount" << endl;
    cout << ">>>-------------------------------------<<<" << endl;

    EXPECT_EQ(0, 0);
}

TEST (PictureParser, Offline) {
    string data_file = "../articles/test_summary.txt";
    string path = "../articles/";
    ifstream file(data_file);
    ofstream out_html("result.html");
    int counter = 0;
    int passed = 0;
    string line = "", filename = "", paper_title = "", cur_title = "";
    vector<string> tmp;
    vector<Requester *> req = {};
    while (file.is_open() && !file.eof()) {
        getline(file, line);
        tmp = split(line, '\t');
        if (tmp.size() < 2) {
            continue;
        }
        filename = tmp[0];
        paper_title = tmp[1];
        string saved_paper_title = raw_to_formatted(paper_title);
        filename = path + filename;
        BiblioManager manager = BiblioManager();
        try {
            ArticleInfo result = manager.search_distance_requesters(req, levenshtein_distance, filename, true);

            manager.print_html(out_html, result);
            paper_title = letters_to_lower(paper_title);
            cur_title = letters_to_lower(result.get_title());
            if (paper_title.find(cur_title) != std::string::npos) {
                passed++;
            } else {
                cout << "Failed at " << filename << endl;
                cout << "Actual: " << saved_paper_title << endl;
                cout << "Got:    " << result.get_title() << endl;
            }
            counter++;
        } catch (const Biblio_exception &e) {
            cerr << e.what() << endl;
        }
    }
    out_html.close();
    cout << ">>>-------------------------------------<<<" << endl;
    cout << "    Passed " << passed << " tests from " << counter << endl;
    cout << "    Passed " << passed * 100 / (float) counter << " % from total amount" << endl;
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

TEST (PictureParser, Dash) {
    ofstream out_html("result.html");
    int counter = 0;
    int passed = 0;
    string line = "", filename = "", paper_title = "", cur_title = "";
    vector<string> tmp;
    vector<Requester *> req = {};

    paper_title = "Solar-Blind UV Photocathodes Based on AlGaN Heterostructures with a 300- to 330-nm Spectral Sensitivity Threshold";
    string saved_paper_title = raw_to_formatted(paper_title);
    filename =  "../articles/test_71.pdf";
    BiblioManager manager = BiblioManager();
    try {
        ArticleInfo result = manager.search_distance_requesters(req, levenshtein_distance, filename, true);

        manager.print_html(out_html, result);
        paper_title = letters_to_lower(paper_title);
        cur_title = letters_to_lower(result[0].get_title());
        if (paper_title.find(cur_title) != std::string::npos) {
            passed++;
        } cout << "Failed at " << filename << endl;
        cout << "Actual: " << saved_paper_title << endl;
        cout << "Got:    " << result[0].get_title() << endl;
        counter++;

        paper_title = "Core-shell monodisperse spherical mSiO2/Gd2O3:Eu3+@mSiO2 particles as potential multifunctional theranostic agents";
        saved_paper_title = raw_to_formatted(paper_title);
        filename = "../articles/test_95.pdf";
        result = manager.search_distance_requesters(req, levenshtein_distance, filename, true);
        manager.print_html(out_html, result);
        paper_title = letters_to_lower(paper_title);
        cur_title = letters_to_lower(result.get_title());
        if (paper_title.find(cur_title) != std::string::npos) {
            passed++;
        } cout << "Failed at " << filename << endl;
        cout << "Actual: " << saved_paper_title << endl;
        cout << "Got:    " << result.get_title() << endl;
        counter++;
    } catch (const Biblio_exception &e) {
        cerr << e.what() << endl;
    }
}
