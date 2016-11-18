#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <gtest/gtest.h>
#include "../src/DBLPManager.h"
#include "../src/BiblioManager.h"
#include "../src/tools.h"
#include "../src/PictureParser.h"

using namespace std;

DBLPManager dblp;
BiblioManager manager;


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST (PaperPresent, Positive) {
    const string query = "Land.Cover.Classification.and.Forest.Change.Analysis";
    const string title = "Land Cover Classification and Forest Change Analysis, Using Satellite Imagery-A Case Study in Dehdez Area of Zagros Mountain in Iran.";
    const string venue = "J. Geographic Information System";
    const string year = "2011";
    vector<ArticleInfo> result = dblp.publicationRequest(query);
    EXPECT_EQ(result[0].get_year(), year);
    EXPECT_EQ(result[0].get_venue(), venue);
    EXPECT_EQ(result[0].get_title(), title);
}

TEST (PaperAbsent, Negative) {
    const string query = "QQQ";
    vector<ArticleInfo> result = dblp.publicationRequest(query);
    EXPECT_EQ(result.size(), 0);
}

TEST (WrongQuery, Negative) {
    const string query = "http://dblp.org/search/pbl/api?q";
    vector<ArticleInfo> result = dblp.publicationRequest(query);
    EXPECT_EQ(result.size(), 0);
}

TEST (TestAlg_TitleExactMatch, Positive) {
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

        filename = tmp[0];
        paper_title = tmp[1];
        filename = path + filename;

        bool offline = false;
        try {
            vector<ArticleInfo> result = manager.search_exact_match(filename, offline);
            if (result.size() > 0) {

                if (low_letters_only(paper_title) == low_letters_only(result[0].get_title())) {
                    passed++;
                } else {
                    cout << "Failed at " << filename << endl;
                }
            } else {
                cout << "Failed at " << filename << endl;
            }
            counter++;
        } catch (const Biblio_exception &e) {
            cerr << e.what() << endl;
        }
    }

    cout << ">>>-------------------------------------<<<" << endl;
    cout << "    Passed " << passed << " tests from " << counter << endl;
    cout << "    Passed " << passed * 100 / (float) counter << " % from total amount" << endl;
    cout << ">>>-------------------------------------<<<" << endl;

    EXPECT_EQ(passed, counter);
}

TEST (TestAlg_TitleLevenshtein, Timing) {
    string data_file = "../articles/test_summary.txt";
    string path = "../articles/";

    ifstream file(data_file);
    int passed = 0;
    int counter = 0;
    string line = "", filename = "", paper_title = "";
    vector<string> tmp;


    ofstream out("before_time.txt");
    out << "--------------------------------" << endl;
    std::clock_t c_start = std::clock();
    std::clock_t sum = 0;
    while (file.is_open() && !file.eof()) {

        std::clock_t start = std::clock();
        getline(file, line);
        tmp = split(line, '\t');

        filename = tmp[0];
        paper_title = tmp[1];
        filename = path + filename;

        bool offline = false;
        try {
            vector<ArticleInfo> result = manager.search_levenshtein_light(filename, offline);
            if (result.size() > 0) {

                if (low_letters_only(paper_title) == low_letters_only(result[0].get_title())) {
                    passed++;
                } else {
                    cout << "Failed at " << filename << endl;
                }
            } else {
                cout << "Failed at " << filename << endl;
            }
            counter++;
        } catch (const Biblio_exception &e) {
            cerr << e.what() << endl;
        }
        out << "................................." << endl;
        std::clock_t end = std::clock();
        sum += 1000.0 * (end - start) / CLOCKS_PER_SEC;

        out << std::fixed << std::setprecision(2) << "CPU time used: "
            << 1000.0 * (end - start) / CLOCKS_PER_SEC << " ms\n";
    }
    out << "--------------------------------" << endl;
    std::clock_t c_end = std::clock();
    out << std::fixed << std::setprecision(2) << "CPU time used: "
        << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << " ms\n";
    out << std::fixed << std::setprecision(2) << "Average CPU time used: "
        << sum / counter << " ms\n";
    cout << ">>>-------------------------------------<<<" << endl;
    cout << "    Passed " << passed << " tests from " << counter << endl;
    cout << "    Passed " << passed * 100 / (float) counter << " % from total amount" << endl;
    cout << ">>>-------------------------------------<<<" << endl;
    out.close();
    EXPECT_EQ(passed, counter);
}

TEST (TestAlg_TitleLevenshtein, Threads_Timing) {
    string data_file = "../articles/test_summary.txt";
    string path = "../articles/";

    ifstream file(data_file);
    int passed = 0;
    int counter = 0;
    string line = "", filename = "", paper_title = "";
    vector<string> tmp;


    ofstream out("before_time.txt");
    out << "--------------------------------" << endl;
    std::clock_t c_start = std::clock();
    std::clock_t sum = 0;
    while (file.is_open() && !file.eof()) {

        std::clock_t start = std::clock();
        getline(file, line);
        tmp = split(line, '\t');

        filename = tmp[0];
        paper_title = tmp[1];
        filename = path + filename;

        bool offline = false;
        try {
            vector<ArticleInfo> result = manager.search_levenshtein_light_threads(filename, offline);
            if (result.size() > 0) {

                if (low_letters_only(paper_title) == low_letters_only(result[0].get_title())) {
                    passed++;
                } else {
                    cout << "Failed at " << filename << endl;
                }
            } else {
                cout << "Failed at " << filename << endl;
            }
            counter++;
        } catch (const Biblio_exception &e) {
            cerr << e.what() << endl;
        }
        out << "................................." << endl;
        std::clock_t end = std::clock();
        sum += 1000.0 * (end - start) / CLOCKS_PER_SEC;

        out << std::fixed << std::setprecision(2) << "CPU time used: "
            << 1000.0 * (end - start) / CLOCKS_PER_SEC << " ms\n";
    }
    out << "--------------------------------" << endl;
    std::clock_t c_end = std::clock();
    out << std::fixed << std::setprecision(2) << "CPU time used: "
        << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << " ms\n";
    out << std::fixed << std::setprecision(2) << "Average CPU time used: "
        << sum / counter << " ms\n";
    cout << ">>>-------------------------------------<<<" << endl;
    cout << "    Passed " << passed << " tests from " << counter << endl;
    cout << "    Passed " << passed * 100 / (float) counter << " % from total amount" << endl;
    cout << ">>>-------------------------------------<<<" << endl;
    out.close();
    EXPECT_EQ(passed, counter);
}

TEST (SimpleParser, Positive) {

    string data_file = "../articles/test_summary.txt";
    string path = "../articles/";

    ifstream file(data_file);

    int passed = 0;
    int counter = 0;

    string line = "", filename = "", paper_title = "", parsed_title = "";
    vector<string> tmp, title;

    while ((file.is_open()) && (!file.eof())) {
        getline(file, line);
        tmp = split(line, '\t');
        filename = tmp[0];
        paper_title = low_letters_only(tmp[1]);
        filename = path + filename;
        vector<string> first_page;
        try {
            Parser pr(filename);
            title = pr.get_title();
            first_page = pr.get_fst_page();
        } catch (const Biblio_exception &e) {
            cerr << e.what() << endl;
            continue;
        }
        parsed_title = "";

        for (string s : title) {
            parsed_title += s + " ";
        }
        parsed_title = low_letters_only(parsed_title);
        if (parsed_title.find(paper_title) != string::npos) {
            passed++;
        } else {
            cout << "In fact: .." << paper_title << ".." << endl;
            cout << "Parsed:  .." << parsed_title << ".." << endl;
        }
        counter++;
    }

    cout << ">>>-------------------------------------<<<" << endl;
    cout << "    Passed " << passed << " tests from " << counter << endl;
    cout << "    Passed " << passed * 100 / (float) counter << " % from total amount" << endl;
    cout << ">>>-------------------------------------<<<" << endl;
    EXPECT_EQ(passed, counter);
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

        filename = tmp[0];
        paper_title = tmp[1];
        filename = path + filename;


        try {

            PictureParser picture_parser = PictureParser(filename, 300, 300, "test.png", "png", 700);
            picture_parser.find_title();
			string result = picture_parser.get_title();

            transform(result.begin(), result.end(), result.begin(), (int (*)(int)) tolower);
            result = raw_to_formatted(result);

            transform(paper_title.begin(), paper_title.end(), paper_title.begin(), (int (*)(int)) tolower);
            paper_title = raw_to_formatted(paper_title);
      
                     
            if (paper_title.find(result) != std::string::npos) {
                passed++;
            } else {
                cout << "Failed at " << filename << endl;
                cout << "Exact paper title: " << endl;
	            cout << paper_title << endl;
                cout << "Parsed title: " << endl;
	            cout << result <<endl;
	            cout <<endl;

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

TEST (PictureParser, Online) {
    string data_file = "../articles/test_summary.txt";
    string path = "../articles/";

    ifstream file(data_file);
    ofstream out_html("result.html");
    int passed = 0;
    int counter = 0;
    string line = "", filename = "", paper_title = "";
    vector<string> tmp;

    while (file.is_open() && !file.eof()) {

        getline(file, line);
        tmp = split(line, '\t');

        filename = tmp[0];
        paper_title = tmp[1];
        filename = path + filename;

        try {

            PictureParser picture_parser = PictureParser(filename, 300, 300, "test.png", "png", 700);
			picture_parser.find_title();
			string result = picture_parser.get_title();
			
            vector<ArticleInfo> result_ = BiblioManager::search_title(result, out_html);

            if (result_.size() > 0) {
                if (low_letters_only(paper_title) == low_letters_only(result_[0].get_title())) {
                    passed++;
                } else {
                    cout << "Failed at " << filename << endl;
                }
            } else {
                cout << "Failed at " << filename << endl;
            }
            if (paper_title.find(result) != std::string::npos) {
                passed++;
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
