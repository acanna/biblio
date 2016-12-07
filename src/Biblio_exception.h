#ifndef BIBLIO_EXCEPTION_H
#define BIBLIO_EXCEPTION_H

#include <string>
#include <exception>

//EL в названиях других классов в этом проекте "_" не используется BiblioException, BiblioFileException
class Biblio_exception : public std::exception {

private:
    std::string _msg;

public:
    Biblio_exception() {}
    //const &
    Biblio_exception(std::string s) : std::exception(), _msg(s) {}
    virtual const char *what() const throw() override { return _msg.c_str(); }
};

class Biblio_file_exception : public Biblio_exception {

private:
    std::string _filename;

public:
    Biblio_file_exception(const std::string &filename) : Biblio_exception(), _filename(filename) {}
    const char *what() const throw() override {
        //EL а зачем здесь static?
        static std::string ex;
        ex = "\nFile " + _filename + " skipped.";
        return ex.c_str();
    }
};

#endif //BIBLIO_EXCEPTION_H
