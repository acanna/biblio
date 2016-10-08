#include <iostream>
#include <algorithm>
#include <regex>
#include "parser.h"
#include "tools.h"

using namespace std;

parser::parser(const string & file_name){
	this->doc = poppler::document::load_from_file(file_name);
	this->fst_page = split(this->doc->create_page(0)->text().to_latin1(),'\n');
	//this->prep = false;
}

vector<string> parser::parse() const{
	vector<string> parsed_text, parsed_page;
	const int pagesNbr = this->doc->pages();
	for (int i = 0; i < pagesNbr; ++i){
		parsed_page = split(this->doc->create_page(i)->text().to_latin1(),'\n');
		parsed_text.resize(parsed_text.size() + parsed_page.size());
		copy_backward(parsed_page.begin(),parsed_page.end(),parsed_text.end());
	}
	return parsed_text;
}

vector<string> parser::get_fst_page() const{
	return this->fst_page;
}
/*
void parser::prep_lns(){
	if(this->prep) return;
		int n = this->fst_page.size(), firstl = 0, lastl = n - 1, contactsl = n - 1, abstractl = n - 1;
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
		regex re_num_word("\\b(\\w+)[0-9]+\\b");
		string formatted = "";
		for(int i = 0; i < n; ++i){
			formatted = regex_replace(this->fst_page[i],re_frmt,frmt_str);
			formatted = regex_replace(formatted,re_space,frmt_space);
			formatted = regex_replace(formatted,re_trim,frmt_trim);
			formatted = regex_replace(formatted,re_sep_word,frmt_join_word);
			formatted = regex_replace(formatted,re_num_word,frmt_num_word);
			this->fst_page[i] = formatted;
			if(regex_search(this->fst_page[i],re_abstr_b_e)){
				lastl = i - 1;
				break;
			}
		
			contacts = false;		
			contacts = this->fst_page[i].find('@') != string::npos || this->fst_page[i].find("email") != string::npos || this->fst_page[i].find("Email") != string::npos;
			//contacts = contacts || fst_page[i].find("University") != string::npos;
			if(contacts){
				contactsl = i;
			}
			if(regex_search(this->fst_page[i],re_abstract)){
				abstractl = i;
			}
			if(contactsl < abstractl && abstractl < n - 1){
				lastl = i - 1;
				break;
			}
		}
		if(contactsl < n - 1)
			lastl = contactsl;
		else if (abstractl < n - 1) lastl = abstractl - 1;
		for (int i = firstl; i <= lastl; ++i){
			if(this->fst_page[i].size() < 4){
				continue;
			}
			prep_lines.push_back(this->fst_page[i]);
		}
		this->prep = true;
	
}
*/

list<string> parser::get_authors() {
	list<string> authors;
	int n = this->fst_page.size(), firstl = 0, lastl = n - 1, contactsl = n - 1, abstractl = n - 1;
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
	regex re_num_word("\\b(\\w+)[0-9]+\\b");
	string formatted = "";
	for(int i = 0; i < n; ++i){
		formatted = regex_replace(this->fst_page[i],re_frmt,frmt_str);
		formatted = regex_replace(formatted,re_space,frmt_space);
		formatted = regex_replace(formatted,re_trim,frmt_trim);
		formatted = regex_replace(formatted,re_sep_word,frmt_join_word);
		formatted = regex_replace(formatted,re_num_word,frmt_num_word);
		this->fst_page[i] = formatted;
		if(regex_search(this->fst_page[i],re_abstr_b_e)){
			lastl = i - 1;
			break;
		}
	
		contacts = false;		
		contacts = this->fst_page[i].find('@') != string::npos || this->fst_page[i].find("email") != string::npos || this->fst_page[i].find("Email") != string::npos;
		//contacts = contacts || fst_page[i].find("University") != string::npos;
		if(contacts){
			contactsl = i;
		}
		if(regex_search(this->fst_page[i],re_abstract)){
			abstractl = i;
		}
		if(contactsl < abstractl && abstractl < n - 1){
			lastl = i - 1;
			break;
		}
	}
	if(contactsl < n - 1)
		lastl = contactsl;
	else if (abstractl < n - 1) lastl = abstractl - 1;
	
	bool cont_name = false;
	string pt_name = "(\\b[:upper:][:alpha:]*\\s+[:upper:]\\.\\s+[:upper:][:alpha:]*\\b|\\b[:upper:]\\.\\s+[:upper:][:alpha:]*\\b)";
	regex re_name(pt_name);
	for (int i = firstl; i <= lastl; ++i){
		if(this->fst_page[i].size() < 4){
			continue;
		}
		cont_name = fst_page[i].find("Dr") != string::npos || fst_page[i].find("PhD") != string::npos || regex_search(fst_page[i],re_name);
		if(cont_name) authors.push_front(fst_page[i]);
		else authors.push_back(fst_page[i]);
		cont_name = false;
	}
	return authors;
}

/*
list<string> parser::get_authors(){
	this->prep_lns();
	list<string> authors;
	int n = this->prep_lines.size();
	//bool cont_name = false;
	//string pt_name = "(\\b[:upper:][:alpha:]*\\s+[:upper:]\\.\\s+[:upper:][:alpha:]*\\b|\\b[:upper:]\\.\\s+[:upper:][:alpha:]*\\b)";
	//regex re_name(pt_name);
	for (int i = 0; i <= n; ++i){
		//cont_name = prep_lines[i].find("Dr") != string::npos || prep_lines[i].find("PhD") != string::npos || regex_search(prep_lines[i],re_name);
		//if(cont_name) authors.push_front(prep_lines[i]);
		//else authors.push_back(prep_lines[i]);
		//cont_name = false;
		authors.push_back(this->prep_lines[i]);
	}	
	return authors;
}
*/
list<string> parser::get_title() {
	return get_authors();
}


vector<poppler::font_info> parser::get_doc_fonts() const{
	vector<poppler::font_info> fonts = doc->fonts();
	return fonts;
}
/*
string parser::toc_title(){
	return doc->create_toc()->root()->title().to_latin1();
}
*/

string parser::get_metadata() const{
	return doc->metadata().to_latin1();
}

