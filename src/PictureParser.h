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
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "BiblioException.h"
#include "tools.h"

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

class Rectangle {
private:
    int x;
    int y;
    int width;
    int height;

public:
    Rectangle (int x, int y, int width, int height);
    int get_x() const;
    int get_y() const;
    int get_height() const;
    int get_width() const;
    static bool _smaller(const Rectangle &rect_1, const Rectangle &rect_2);
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
    int title_width;
    std::string title;

private:
    bool is_black(int x, int y);
    bool is_white(int x, int y);
    bool white_background(int x_start, int y);
    void draw_title_rectangle();
    void select_title_rectangle(std::vector<Rectangle> & areas);
    std::vector<Rectangle> select_areas(cv::Mat & rgb);
    std::string parse_image();
    void make_white_rectangle (int x, int y, int width, int height);

public:
    PictureParser() {};
    ~PictureParser();
    PictureParser(std::string const &filename, int const xres, int const yres,
                  std::string const &imagename, std::string const &format, int const dpi);
    const std::string& find_title();
};

