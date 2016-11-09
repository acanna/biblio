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

string delete_multiple_spaces(std::string str) {
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

string raw_to_formatted(string raw){
	string frmt_str = "";
	string frmt_space = " ";
	string frmt_trim = "$1";
	string frmt_join_word = "$1$2";
	string frmt_num_word = "$1";
	regex re_space("\\b\\s+\\b");
	regex re_frmt("[^\\w\\.,-@\\s]+");
	regex re_trim("^\\s*(.*)\\s*$");
	regex re_sep_word("\\b([b-zB-Z])\\s(\\w+)\\b");
	regex re_num_word("\\b([a-zA-Z]+)[0-9]+\\b");
	
	string result = regex_replace(raw,re_frmt,frmt_str);
	result = regex_replace(result,re_space,frmt_space);
	result = regex_replace(result,re_trim,frmt_trim);
	result = regex_replace(result,re_sep_word,frmt_join_word);
	result = regex_replace(result,re_num_word,frmt_num_word);
	
	return result;
}

string delete_junk_symbol(string str) {
	transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
	replace(str.begin(), str.end(), '.', ' ');
	str.erase(str.find_last_not_of(" \n\r\t")+1);
	return str;
}

/*
int levenshtein_distance(const std::string & s, const std::string & t) {

	if (s == t) return 0;
	size_t t_len = t.length();
	size_t s_len = s.length();
	if (s.length() == 0) return t_len;
	if (t.length() == 0) return s_len;

	int v0[t_len+1];
	int v1[t_len+1];

	for (size_t i = 0; i < t.length() + 1; i++) {
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
	return v1[t_len];
}
 */


size_t damerauLevenshteinDistance_beg(const string & a, size_t ind_beg, const string & b) {

	size_t b_size = b.length();
	size_t a_size = b_size;

	size_t INF = 2 * b_size;
	vector<vector<size_t>> H(a_size + 2, vector<size_t>(b_size + 2, 0));
	H[0][0] = INF;
	for (size_t i = 0; i <= a_size; ++i) {
		H[i+1][1] = i;
		H[i+1][0] = INF;
	}
	for (size_t j = 0; j <= b_size; ++j) {
		H[1][j+1] = j;
		H[0][j+1] = INF;
	}

	const int alphabet_size = numeric_limits<u_char>::max();
	vector<u_char> DA(alphabet_size, 0);
	for (size_t i = 1; i <= a_size; ++i) {
		size_t DB = 0;
		for (size_t j = 1; j <= b_size; ++j) {
			size_t i1 = DA[(u_char)b[j - 1]];
			size_t j1 = DB;
			int d = (a[ind_beg + i - 1] == b[j - 1]) ? 0 : 1;
			if (d == 0) {
				DB = j;
			}
			H[i + 1][j + 1] = min(min(H[i][j] + d, H[i + 1][j] + 1),
								  min(H[i][j + 1] + 1, H[i1][j1] + (i - i1 - 1) + 1 + (j - j1 - 1)));
		}
		DA[(u_char)a[ind_beg + i - 1]] = (u_char)i;
	}
	return H[a_size + 1][b_size + 1];
}

size_t damerauLevenshteinDistance_biblio(const string & a, const string & b) {
	size_t a_size = a.length();
	size_t b_size = b.length();
	if (a_size < b_size) {
		return b_size;
	}

	size_t dist = a_size;

	for (size_t ind_beg = 0; ind_beg <= a_size - b_size; ind_beg++) {
		dist = min(dist, damerauLevenshteinDistance_beg(a, ind_beg, b));
	}

	return dist;
}

size_t damerauLevenshteinDistance(const std::string &a, const std::string &b) {
	size_t b_size = b.length();
	size_t a_size = a.length();

	size_t INF = a_size + b_size;
	vector<vector<size_t>> H(a_size + 2, vector<size_t>(b_size + 2, 0));
	H[0][0] = INF;
	for (size_t i = 0; i <= a_size; ++i) {
		H[i+1][1] = i;
		H[i+1][0] = INF;
	}
	for (size_t j = 0; j <= b_size; ++j) {
		H[1][j+1] = j;
		H[0][j+1] = INF;
	}

	const int alphabet_size = numeric_limits<u_char>::max();
	vector<u_char> DA(alphabet_size, 0);
	for (size_t i = 1; i <= a_size; ++i) {
		size_t DB = 0;
		for (size_t j = 1; j <= b_size; ++j) {
			size_t i1 = DA[(u_char)b[j - 1]];
			size_t j1 = DB;
			int d = (a[i - 1] == b[j - 1]) ? 0 : 1;
			if (d == 0) {
				DB = j;
			}
			H[i + 1][j + 1] = min(min(H[i][j] + d, H[i + 1][j] + 1),
								  min(H[i][j + 1] + 1, H[i1][j1] + (i - i1 - 1) + 1 + (j - j1 - 1)));
		}
		DA[(u_char)a[i - 1]] = (u_char)i;
	}
	return H[a_size + 1][b_size + 1];
}

size_t levenshtein_distance(const string & src, const string & dst) {
	const size_t m = src.size();
	const size_t n = dst.size();
	if (m == 0) {
		return n;
	}
	if (n == 0) {
		return m;
	}

	std::vector< std::vector<size_t>> matrix(m + 1);

	for (size_t i = 0; i <= m; ++i) {
		matrix[i].resize(n + 1);
		matrix[i][0] = i;
	}
	for (size_t i = 0; i <= n; ++i) {
		matrix[0][i] = i;
	}

	size_t above_cell, left_cell, diagonal_cell, cost;

	for (size_t i = 1; i <= m; ++i) {
		for(size_t j = 1; j <= n; ++j) {
			cost = src[i - 1] == dst[j - 1] ? 0 : 1;
			above_cell = matrix[i - 1][j];
			left_cell = matrix[i][j - 1];
			diagonal_cell = matrix[i - 1][j - 1];
			matrix[i][j] = std::min(std::min(above_cell + 1, left_cell + 1), diagonal_cell + cost);
		}
	}
	return matrix[m][n];
}