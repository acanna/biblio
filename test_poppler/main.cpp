#include <iostream>
#include <list>
#include "parser.h"

using namespace std;

int main(int argc, char ** argv)
{
	parser * pr;
	string file_name; 
	if (argc > 1){
		try{
		file_name = string(argv[1]);
		pr = new parser(file_name);
		} catch(...) {}
	} else {		
		cin >> file_name;
		pr = new parser(file_name);
    }
    list<string> auths = pr->get_authors();
    for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it){
    	cout << *it << endl;
    }
    
    delete pr;
}
