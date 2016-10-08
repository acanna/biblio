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
    /*
	vector<poppler::font_info> fonts = pr->get_doc_fonts();
	cout << "metadata" << endl;
	cout << pr->get_metadata() << endl;
	
    cout << "fonts" << endl;
	int k = fonts.size();
	for(int i = 0; i < k; ++i)
        cout << fonts[i].name()<< " " << fonts[i].type() << endl;
        
	cout << "fst_page" << endl;
	int n = lines.size();
    for(int i = 0; i < n; ++i)
        cout << lines[i] << endl;
    */
    list<string> auths = pr->get_authors();
    for (list<string>::const_iterator it = auths.begin(); it != auths.end(); ++it){
    	cout << *it << endl;
    }
    
    delete pr;
}
