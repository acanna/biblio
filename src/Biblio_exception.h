#ifndef BIBLIO_EXCEPTION_H
#define BIBLIO_EXCEPTION_H

#include <string>
#include <exception>

class Biblio_exception : public std::exception {

public:

	Biblio_exception() {}

	Biblio_exception(std::string s) : std::exception(), _msg(s) {}

	virtual const char* what() const throw() override {
		return _msg.c_str();
	}

private:

	std::string _msg;
};

class Biblio_file_exception : public Biblio_exception {

public:

	Biblio_file_exception( const std::string& filename) : Biblio_exception(), _filename(filename) { }

	const char* what() const throw() override {
		static std::string ex;
		ex = "\nFile " + _filename + " skipped.";
		return ex.c_str();
	}

private:

	std::string _filename;
};

#endif //BIBLIO_EXCEPTION_H
