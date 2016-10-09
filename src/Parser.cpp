#include <iostream>
#include <algorithm>
#include <regex>
#include "Parser.h"
#include "tools.h"

using namespace std;

Parser::Parser(const string & file_name){
	/*
	try{
		doc = poppler::document::load_from_file(file_name);
		fst_page = split(this->doc->create_page(0)->text().to_latin1(),'\n');
	} catch(...) {}
	*/
	// TODO: add throw
	
	doc = poppler::document::load_from_file(file_name);
	fst_page = split(doc->create_page(0)->text().to_latin1(),'\n');
	
	vector<string> modified;
	int n = fst_page.size();
	n = min(n,10);
	int firstl = 0, lastl = n - 1, contactsl = n - 1, abstractl = n - 1;
	bool contacts = false;
	string pt_year = "\\b(19|20)\\d{2}\\b";
	string pt_pages = "\\b\\d+-\\d+\\b";
	string pt_abstract = "\\bA\\s*(B|b)\\s*(S|s)\\s*(T|t)\\s*(R|r)\\s*(A|a)\\s*(C|c)\\s*(T|t)";
	string pt_abstr_b_e = "^\\bA\\s*(B|b)\\s*(S|s)\\s*(T|t)\\s*(R|r)\\s*(A|a)\\s*(C|c)\\s*(T|t)\\s*\\b$";
	string frmt_str = "";
	string frmt_space = " ";
	string frmt_trim = "$1";
	regex re_space("\\b\\s+\\b");
	regex re_frmt("[^\\w\\.,-@\\s]+");
	regex re_trim("^\\s*(.*)\\s*$");
	regex re_year(pt_year);
	regex re_pages(pt_pages);
	regex re_abstract(pt_abstract);
	regex re_abstr_b_e(pt_abstr_b_e);
	string frmt_join_word = "$1$2";
	regex re_sep_word("\\b([b-zB-Z])\\s(\\w+)\\b");
	string frmt_num_word = "$1";
	regex re_num_word("\\b([a-zA-Z]+)[0-9]+\\b");
	string frmt_email = " ";
	regex re_email("\\b(email|Email|EMAIL)?:?\\s*[\\w\\.,]*@[\\w\\.]*\\s*\\b");
	regex re_date("\\b(\\d{2}\\s+\\w+|\\w+\\s+\\d{2}(|,))\\s+(19|20)\\d{2}\\b");
	regex re_dept("\\b(Dept|DEPT)\\.\\b");
	string formatted = "";
	for (int i = 0; i < n; ++i) {
		formatted = regex_replace(fst_page[i],re_frmt,frmt_str);
		formatted = regex_replace(formatted,re_space,frmt_space);
		formatted = regex_replace(formatted,re_trim,frmt_trim);
		formatted = regex_replace(formatted,re_sep_word,frmt_join_word);
		formatted = regex_replace(formatted,re_num_word,frmt_num_word);
		fst_page[i] = formatted;
		if (regex_search(fst_page[i],re_abstr_b_e)) {
			lastl = i - 1;
			break;
		}	
		contacts = false;
		contacts = fst_page[i].find('@') != string::npos || fst_page[i].find("email") != string::npos || fst_page[i].find("Email") != string::npos;
		if (contacts) {
			contactsl = i;
		}
		if (regex_search(fst_page[i],re_abstract)) {
			abstractl = i;
		}
		if (contactsl < abstractl && abstractl < n - 1) {
			lastl = i - 1;
			break;
		}
	}
	if (contactsl < n - 1) {
		lastl = contactsl;
	}
	else if (abstractl < n - 1) {
		lastl = abstractl - 1;
	}
	for (int i = firstl; i <= lastl; ++i) {
		if (regex_search(fst_page[i],re_date) || regex_search(fst_page[i],re_dept)) {
			continue;
		}
		formatted = regex_replace(fst_page[i],re_email,frmt_email);
		formatted = regex_replace(formatted,re_space,frmt_space);
		formatted = regex_replace(formatted,re_trim,frmt_trim);
		fst_page[i] = formatted;
		if (fst_page[i].size() < 4) {
			continue;
		}
		modified.push_back(fst_page[i]);
	}
	fst_page = modified;
}

vector<string> Parser::get_document() const{
	vector<string> parsed_text, parsed_page;
	const int pages_nbr = doc->pages();
	for (int i = 0; i < pages_nbr; ++i) {
		parsed_page = split(doc->create_page(i)->text().to_latin1(),'\n');
		parsed_text.resize(parsed_text.size() + parsed_page.size());
		copy_backward(parsed_page.begin(),parsed_page.end(),parsed_text.end());
	}
	return parsed_text;
}

vector<string> Parser::get_fst_page() const{
	return this->fst_page;
}

list<string> Parser::get_authors() const{
	list<string> authors;
	int n = fst_page.size();
	bool cont_name = false;
	string pt_name = "(\\b[:upper:][:alpha:]*\\s+[:upper:]\\.\\s+[:upper:][:alpha:]*\\b|\\b[:upper:]\\.\\s+[:upper:][:alpha:]*\\b)";
	regex re_name(pt_name);
	for (int i = 0; i < n; ++i) {
		cont_name = fst_page[i].find("Dr") != string::npos || fst_page[i].find("PhD") != string::npos || regex_search(fst_page[i],re_name);
		if (cont_name) {
			authors.push_front(fst_page[i]);
		} else {
			authors.push_back(fst_page[i]);
		}
		cont_name = false;
	}
	return authors;
}

list<string> Parser::get_title() const{
	return get_authors();
}


