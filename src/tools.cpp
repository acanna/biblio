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

