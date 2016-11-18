#pragma once

#include <iostream>
#include <algorithm>
#include <fstream>
#include <regex>
#include <vector>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h> 
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-rectangle.h>
#include <poppler/cpp/poppler-font.h>

#include "Biblio_exception.h"
#include "tools.h"

/*
class TitleRect {
private:
   int title_x;
   int title_y;
  int title_height;
	int rate;

public:
	TitleRect(int const title_x, int const title_y, int const title_height);
	int const &get_x();
	int const &get_y();
	int const &get_height();
	int const &get_rate();
	void set_rate(int rate);
};*/

/*
class PixInfo {

private:
    int x;
    int y;
	int row_height;

public:
    PixInfo(int const x, int const y, int const row_height);
	int get_height() const;
    int get_x() const;
    int get_y() const;
	static bool is_greater(const PixInfo &pi_1, const PixInfo &pi_2);
};
*/

class PixInfo {

private:
    int x;
    int y;

private:
    std::vector<int> row_height;

public:
    PixInfo(int const x, int const y, std::vector<int> const &row_height);
    std::vector<int> *get_heights();
    int get_x() const;
    int get_y() const;
};

class PictureParser {

private:
    std::string filename;
    std::string imagename;
    int xres;
    int yres;
    int dpi;
    int width;
    int height;
    std::string format;
    char *data;
	int title_x;
    int title_y;
    int title_height;
    std::string title;

private:
    bool is_black(int x, int y);
	bool is_dark(int x, int y);
    bool is_white(int x, int y);
    bool white_background(int x_start, int y);
    int check_b_w_pix_ratio(int title_x, int title_y, int title_height);
    void select_title_rectangle();
    std::string parse_image();

public:
    PictureParser(std::string const &filename, int const xres, int const yres,
                  std::string const &imagename, std::string const &format, int const dpi);
    std::string const &get_title();
    void find_title();
};

