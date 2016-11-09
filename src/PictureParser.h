#pragma once

#include <iostream>
#include <algorithm>
#include <fstream>
#include <regex>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h> 
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-rectangle.h>

#include "Biblio_exception.h"
#include "tools.h"



class PictureParser{
	std::string filename;  
	std::string imagename;
    int xres;
    int yres;
    int dpi;
    std::string format;
	char * data;
	int width;
	int height;
	int title_x;
	int title_y;
	int title_height;
	std::string title;

	bool is_black(int x, int y);
	void select_title_rectangle();
    std::string parse_image();



public: 
    PictureParser(std::string const & filename, int const xres, int const yres, 
				  std::string const & imagename, std::string const & format, int const dpi);
	std::string const & get_title();
//	template<typename T>
    std::string find_title ();

};

class PixInfo{
	int x;
	int y;
	std::vector <int> row_height;

public:	
	PixInfo(int const x, int const y, std::vector <int> const & row_height);
	std::vector<int> * get_heights();
	int const & get_x();
	int const & get_y();
};