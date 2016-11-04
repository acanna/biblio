#include <regex>
#include "tools.h"

using namespace std;

vector<string> split(const string & str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // создаём поток из строки
	string tok;  
	while (getline(ss, tok, delimiter)) {
		if (tok.size() > 0) {
			internal.push_back(tok);
		}
	}
	return internal;
}

string low_letters_only(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    regex re_frmt("[^a-zA-Z]");
    regex re_space("\\b\\s+\\b");
    str = regex_replace(str, re_frmt, "");
    str = regex_replace(str, re_space, " ");
    return str;
}

std::string delete_multiple_spaces(std::string str) {
	regex re_frmt("\\b\\s+\\b");
	return regex_replace(str, re_frmt, " ");
}

string short_name(string s) {
	regex re_frmt("[^a-zA-Z\\s]");
	regex re_name("\\b([a-zA-Z]{2,3})[a-zA-Z]*");
	string res = regex_replace(s, re_frmt, "");
	regex re_let("\\b[a-zA-Z]\\b");
	res = regex_replace(res, re_let, "");
	res = regex_replace(res, re_name, "$1");
	regex re_trim("^\\s*\\b(.*)\\b\\s*$");
	res = regex_replace(res, re_trim, "$1");
	regex re_one("[a-zA-Z]+\\s");
	res = regex_replace(res, re_one, "");
	res = regex_replace(res, re_trim, "$1");
	return res;
}

string delete_junk_symbol(string str) {
	transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
	replace(str.begin(), str.end(), '.', ' ');
	str.erase(str.find_last_not_of(" \n\r\t")+1);
	return str;
}

int levenshtein_distance(const std::string & s, const std::string & t) {

	if (s == t) return 0;
	if (s.length() == 0) return t.length();
	if (t.length() == 0) return s.length();

	int v0[t.length()+1];
	int v1[t.length()+1];
	for (unsigned int i = 0; i < t.length() + 1; i++) {
		v0[i] = i;
	}
	for (unsigned int i = 0; i < s.length(); i++){
		v1[0] = i + 1;
		for (unsigned int j = 0; j < t.length(); j++) {
			int cost = (s[i] == t[j]) ? 0 : 1;
			v1[j + 1] = min(v1[j] + 1, min(v0[j + 1] + 1, v0[j] + cost));
		}
		for (unsigned int j = 0; j < t.length() + 1; j++)
			v0[j] = v1[j];
	}
	return v1[t.length()];
}