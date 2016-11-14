#include <regex>
#include "tools.h"

using namespace std;

vector<string> split(const string &str, char delimiter) {
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

string delete_multiple_spaces(string str) {
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

string raw_to_formatted(string raw) {
    string frmt_str = "";
    string frmt_space = " ";
    string frmt_trim = "$1";
    string frmt_join_word = "$1$2";
    string frmt_num_word = "$1";
    regex re_frmt_sp("\\b\\s+\\b");
    regex re_space("\\b\\s+\\b");
    regex re_trim_space("^ +| +$|( ) +");
    regex re_frmt("[^\\w\\.,-@\\s]+");
    regex re_trim("^\\s*(.*)\\s*$");
    regex re_sep_word("\\b([b-zB-Z])\\s(\\w+)\\b");
    regex re_num_word("\\b([a-zA-Z]+)[0-9]+\\b");

    string result = regex_replace(raw, re_frmt, frmt_str);
    result = regex_replace(result, re_frmt_sp, frmt_space);
    result = regex_replace(result, re_trim_space, frmt_trim);
    result = regex_replace(result, re_space, frmt_space);
    result = regex_replace(result, re_trim, frmt_trim);
    result = regex_replace(result, re_sep_word, frmt_join_word);
    result = regex_replace(result, re_num_word, frmt_num_word);
    result.erase(remove(result.begin(), result.end(), '\n'), result.end());

    return result;
}

string delete_junk_symbol(string str) {
    transform(str.begin(), str.end(), str.begin(), (int (*)(int)) tolower);
    replace(str.begin(), str.end(), '.', ' ');
    str.erase(str.find_last_not_of(" \n\r\t") + 1);
    return str;
}


size_t levenshtein_distance(const string &s, const string &t) {
    if (s == t) return 0;
    size_t t_length = t.length();
    size_t s_length = s.length();
    if (s.length() == 0) return t_length;
    if (t.length() == 0) return s_length;

    size_t max_length = max(t_length, s_length);
    size_t v0[max_length];
    size_t v1[max_length];
    for (size_t i = 0; i < max_length; i++) {
        v0[i] = i;
    }
    for (size_t i = 0; i < s_length; i++) {
        v1[0] = i + 1;
        for (size_t j = 0; j < t_length; j++) {
            int cost = (s[i] == t[j]) ? 0 : 1;
            v1[j + 1] = min(v1[j] + 1, min(v0[j + 1] + 1, v0[j] + cost));
            if (v1[j + 1] >= 15) {
                return max_length;
            }
        }
        for (size_t j = 0; j < max_length; j++)
            v0[j] = v1[j];
    }
    return v1[t_length];
}