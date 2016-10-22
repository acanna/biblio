#ifndef BIBLIO_EXCEPTION_H
#define BIBLIO_EXCEPTION_H

#include <exception>

class Biblio_exception : public std::exception {

};

class Biblio_file_exception : public Biblio_exception {

};


#endif //BIBLIO_EXCEPTION_H
