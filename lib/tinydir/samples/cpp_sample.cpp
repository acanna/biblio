#include <iostream>
#include <stdexcept>
#include <string>

struct tinydir_dir;

class TinyDir {
public:
    TinyDir(const std::string& path);
    ~TinyDir();
    std::string BaseName() const;
private:
    tinydir_dir* dir;
};

#include <tinydir.h>

TinyDir::TinyDir(const std::string& path) : dir(new tinydir_dir) {
    if (tinydir_open(dir, path.c_str()) == -1) {
        throw std::invalid_argument{"path"};
    }
}

TinyDir::~TinyDir() {
    delete dir;
}

std::string TinyDir::BaseName() const {
    const std::string path{dir->path};
    auto lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos) {
        return path;
    }
    return path.substr(lastSlash + 1);
}

void recursive_print_dir(std::string path) {
    tinydir_dir dir;
    tinydir_open(&dir, path.c_str());
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        printf("%s", file.name);
        if (file.is_dir)
        {
            printf("/");
            recursive_print_dir(path + file.name);
        }
        printf("\n");

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: cpp_sample filename\n";
        return 1;
    }
//    TinyDir td{argv[1]};
//    std::cout << "Basename is " << td.BaseName() << "\n";

    /*
    tinydir_dir dir;
    tinydir_open(&dir, argv[1]);

    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        printf("%s", file.name);
        if (file.is_dir)
        {
            printf("/");
        }
        printf("\n");

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
     */
    recursive_print_dir(std::string(argv[1]));
    return 0;
}

