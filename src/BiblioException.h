#ifndef BIBLIO_EXCEPTION_H
#define BIBLIO_EXCEPTION_H

#include <string>
#include <exception>

class BiblioException : public std::exception {

private:
    std::string _msg;

public:
    BiblioException() {}
    BiblioException(const std::string & s) : std::exception(), _msg(s) {}
    virtual const char *what() const throw() override { return _msg.c_str(); }
};

class BiblioFileException: public BiblioException {

private:
    std::string _filename;

public:
    BiblioFileException(const std::string &filename) : BiblioException(), _filename(filename) {}
    const char *what() const throw() override {
        std::string ex;
        ex = "\nFile " + _filename + " skipped.";
        return ex.c_str();
    }
};

#endif //BIBLIO_EXCEPTION_H
