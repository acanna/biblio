#include "tools.h"
#include <sstream>
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

