#include <iostream>
#include "parser.h"
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
using namespace std;

int main()
{
    string file_name;
    cin >> file_name;
    poppler::document *doc = poppler::document::load_from_file("./articles/" + file_name);
    const int pagesNbr = doc->pages();

    for (int i = 0; i < pagesNbr; ++i)
        cout << doc->create_page(i)->text().to_latin1().c_str() << endl;
}
