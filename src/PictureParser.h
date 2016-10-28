#pragma once

#include <iostream>
#include <algorithm>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h> 
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>


class PictureParser{
	std::string filename;  
	std::string imagename;
    int xres;
    int yres;
    int dpi;
    std::string format;

public: 
    PictureParser(std::string filename, int xres, int yres, std::string imagename, std::string format, int dpi);
    void save_as_image (int page_num);
    char* parse_image();

};