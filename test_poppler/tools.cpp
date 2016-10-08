#include "tools.h"

using namespace std;

vector<string> split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // создаём поток из строки
  string tok;
  
  while(getline(ss, tok, delimiter)) {
  	if(tok.size() > 0)
    	internal.push_back(tok);
  }
  
  return internal;
}

string trim(string s){
	return s;
}
