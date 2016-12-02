#include <regex>
#include <iostream>
#include <libconfig.h++>
#include "tools.h"
#include "../lib/tinydir/tinydir.h"

using namespace std;
using namespace libconfig;

vector<Requester *> read_config(const string &filename){
	Config cfg;

	try {	
    	cfg.readFile(filename.c_str());
    } catch (const FileIOException &ex){
		throw Biblio_exception("Reading config file failed");
	}  catch (const ParseException &pex){
		string what = "Parse error at " + (string)pex.getFile() + ":" 
			+ to_string(pex.getLine()) + " - " + (string)pex.getError();
		throw Biblio_exception("Reading config file failed: "+ what);
	}

	vector<Requester*> active_requesters = {};
	try {
		if (cfg.lookup("dblp.enabled")){
			string url = cfg.lookup("dblp.url");
			Requester * dblp = new DBLPRequester(url);
			active_requesters.push_back(dblp);
		}
		if (cfg.lookup("springer.enabled")){
			string url = cfg.lookup("springer.url");
			string apikey = cfg.lookup("springer.apikey");
			Requester * springer = new SpringerRequester(url, apikey);
			active_requesters.push_back(springer);
		}
		if (cfg.lookup("arxiv.enabled")){
			string url = cfg.lookup("arxiv.url");
			Requester * arxiv = new ArxivRequester(url);
			active_requesters.push_back(arxiv);
		}
	}
	catch (const SettingNotFoundException &nfex){
	    throw Biblio_exception("Config file has wrong format");;
	}
    return active_requesters;
}

vector<string> split(const string &str, char delimiter) {
    vector<string> internal;
    stringstream ss(str); 
    string tok;
    while (getline(ss, tok, delimiter)) {
        if (tok.size() > 0) {
            internal.push_back(tok);
        }
    }
    return internal;
}

string low_letters_only(string str) {
    regex re_frmt("[^a-zA-Z]");
    str = regex_replace(str, re_frmt, "");
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
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
    regex re_endl("[\\n\\r]+");
    regex re_space("\\b\\s+\\b");
    regex re_frmt("[^\\(\\)\\{\\}\\[\\]\\w\\.,_:@\\s\\+\\-]+");
    regex re_trim("^\\s*\\b(.*)\\b\\s*$");
    string result = regex_replace(raw, re_endl, " ");
    result = regex_replace(result, re_frmt, "");
    result = regex_replace(result, re_space, " ");
    result = regex_replace(result, re_trim, "$1");

    return result;
}

size_t levenshtein_distance(const string &s, const string &t) {
    if (s == t) return 0;
    size_t t_length = t.length();
    size_t s_length = s.length();
    if (s.length() == 0) return t_length;
    if (t.length() == 0) return s_length;

    size_t max_length = max(t_length, s_length);
    vector<size_t> v0(max_length);
    vector<size_t> v1(max_length);
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

std::string delete_spaces_to_lower(std::string str) {
    transform(str.begin(), str.end(), str.begin(), (int (*)(int)) tolower);
    regex re_space("\\b\\s+\\b");
    return regex_replace(str, re_space, "");
}

std::vector<std::string> read_pdf_files_recursive(std::string &path) {
    vector<string> files = {};
    read_pdf_files(files, path);
    return files;
}

void read_pdf_files(std::vector<std::string> &v, std::string &path) {
    if (path[path.length() - 1] != '/') {
        path += "/";
    }
    tinydir_dir dir;
    tinydir_open(&dir, path.c_str());
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        string filename = string(file.name);
        if (file.is_dir)
        {
            if (filename != "." && filename != ".."){
                filename = path + filename;
                read_pdf_files(v, filename);
            }
        } else {
            if (regex_match(filename,regex(".*\\.pdf$"))) {
                v.push_back(path + filename);
            }
        }
        tinydir_next(&dir);
    }
    tinydir_close(&dir);
}

std::string letters_to_lower(std::string s) {
    regex re_space("\\b\\s+\\b");
    regex re_frmt("[^\\w,\\s]+|[\\n\\r]+");
    regex re_trim("^\\s*\\b(.*)\\b\\s*$");
    string result = regex_replace(s, re_frmt, "");
    result = regex_replace(result, re_space, " ");
    result = regex_replace(result, re_trim, "$1");
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
