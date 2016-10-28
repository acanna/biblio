#ifndef BIBLIO_EXCEPTION_H
#define BIBLIO_EXCEPTION_H

#include <string>
#include <exception>

class Biblio_exception : public std::exception {

public:

	Biblio_exception( const std::string& filename) : std::exception(), _filename(filename) { }
	
	virtual ~Biblio_exception() throw() { }
	
	const char* what() const throw() 
		{
			static std::string ex; 
			ex = "\nSkipped file " + _filename;
			return ex.c_str();
		}
		
private:

	std::string _filename;

};

#endif //BIBLIO_EXCEPTION_H
