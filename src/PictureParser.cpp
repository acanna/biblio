#include "PictureParser.h"

using namespace std;
using namespace cv;

//------------------------ >> PixInfo <<----------------------------------//

PixInfo::PixInfo(int const x, int const y, std::vector<int> const &row_height) {
    this->x = x;
    this->y = y;
    this->row_height = row_height;
}

vector<int> *PixInfo::get_heights() {
    return &this->row_height;
}

int PixInfo::get_x() const {
    return this->x;
}

int PixInfo::get_y() const {
    return this->y;
}

//------------------------ >> Rectangle <<----------------------------------//

Rectangle::Rectangle (int x, int y, int width, int height){
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

int Rectangle::get_x() const {
	return this->x;
}

int Rectangle::get_y() const {
	return this->y;
}

int Rectangle::get_height() const {
	return this->height;
}

int Rectangle::get_width() const {
	return this->width;
}

bool Rectangle::_smaller(const Rectangle &rect_1, const Rectangle &rect_2) {
	if (rect_1.get_y() != rect_2.get_y()) {
    	return rect_1.get_y() < rect_2.get_y();
	} else {
    	return rect_1.get_x() < rect_2.get_x();
	}
}

//------------------------ >> PictureParser <<----------------------------------//

PictureParser::PictureParser(string const &filename, int const xres, int const yres,
                             string const &imagename, string const &format, int const dpi) {

    this->filename = filename;
    this->xres = xres;
    this->yres = yres;
    this->imagename = imagename;
    this->format = format;
    this->dpi = dpi;
    this->data = nullptr;
    this->title = "";
}

string const &PictureParser::get_title() {
    return this->title;
}

bool PictureParser::is_black(int x, int y) {
    int black = 60; // (color <= black) <=> (black pix)
				    // (color > black) <=> (white pix)

    //	avoid other colors
    if ((((this->data[y * (this->width) * 4 + x * 4]) & 255) > black) ||
        (((this->data[y * (this->width) * 4 + x * 4 + 1]) & 255) > black) ||
        (((this->data[y * (this->width) * 4 + x * 4 + 2]) & 255) > black))
        return false;

    return ( ((int) (this->data[y * (this->width) * 4 + x * 4]) & 255) *
             ((int) (this->data[y * (this->width) * 4 + x * 4 + 3]) & 255) / 255 <= black);

}

bool PictureParser::is_white(int x, int y) {
    int white = 230; // (color >= white) <=> (white pix)
				     // (color < white) <=> (black pix)

    return ( ((int) (this->data[y * (this->width) * 4 + x * 4]) & 255) *
             ((int) (this->data[y * (this->width) * 4 + x * 4 + 3]) & 255) / 255 >= white);
}

bool PictureParser::white_background(int x_start, int y) {
    int white_counter = 0;
	int const enough_white_space = 10;

    for (int x = x_start; x < (int)(this->width / 2); x++){
        if (is_white(x, y)) {
            white_counter++;
        }

        if (white_counter > enough_white_space) {
            return true;
        }
    }
    return false;
}

void PictureParser::draw_title_rectangle(){
	int const white = 115;
	int x = this->title_x;
	int y = this->title_y;
	int width = this->title_width;
	int height = this->title_height;

	for (int x_ = x; x_ < x + width; x_++){
		for (int y_ = y; y_ < y + height; y_++){
			this->data[y_ * (this->width) * 4 + x_ * 4] = white;
	        this->data[y_ * (this->width) * 4 + x_ * 4 + 1] = white;
    	    this->data[y_ * (this->width) * 4 + x_ * 4 + 2] = white;
		}
	}

}


void PictureParser::find_title() {
    int const screen_res = 72;
    int page_num = 0;
    string result = "";

    poppler::document *doc = poppler::document::load_from_file(this->filename);
    const int pages_nbr = doc->pages();
    if (page_num > pages_nbr) {
        throw BiblioException("PictureParser: Wrong page number indicated.\n");
    }
    poppler::page *mypage = doc->create_page(page_num);

    this->width = int(mypage->page_rect().width() * (this->xres) / screen_res);
    this->height = int(mypage->page_rect().height() * (this->yres) / screen_res);

    poppler::page_renderer renderer;
    poppler::image cur_image = renderer.render_page(mypage,
                         this->xres, this->yres, 0, 0, this->width, this->height);        
	cur_image.save(this->imagename, this->format, this->dpi);


    this->data = cur_image.data();

	//	select areas with text using opencv-lib
    Mat rgb = imread(this->imagename);
	vector<Rectangle> areas = select_areas (rgb);

	// specify title area using row-by-row pix scanning
    select_title_rectangle(areas);

	poppler::image mod_image =  poppler::image(this->data, cur_image.width(), 
			cur_image.height(), cur_image.format());
	mod_image.save("_"+this->imagename, this->format, this->dpi);


	vector<poppler::font_info> fonts = doc->fonts();
	// pdf as image
    if (fonts.size() == 0) {
        poppler::image rect_image = renderer.render_page(mypage,
                         this->xres, this->yres, this->title_x, this->title_y, this->title_width, this->title_height);
        rect_image.save(this->imagename, this->format, this->dpi);
        result = parse_image();

    // pdf as text
    } else {
        double x_rect = this->title_x * screen_res / this->xres;
        double y_rect = this->title_y * screen_res / this->yres;
        double height_rect = this->title_height * screen_res / this->yres;
        double width_rect = this->title_width * screen_res / this->xres;
        poppler::rectangle<double> rect = poppler::rectangle<double>(x_rect, 
											y_rect, width_rect, height_rect);
        result = mypage->text(rect).to_latin1();
    }
    result = raw_to_formatted(result);
    this->title = result;
}

vector<Rectangle> PictureParser::select_areas(Mat & rgb_){

    // downsample and use it for processing
	Mat rgb;
    pyrDown(rgb_, rgb);

	double coefToPopplerX = 1.0*this->width/rgb.size().width;
	double coefToPopplerY = 1.0*this->height/rgb.size().height;

	vector<Rectangle> areas = {};
    Mat small;
    cvtColor(rgb, small, CV_BGR2GRAY);

    // morphological gradient
    Mat grad;
    Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);

    // binarize
    Mat bw;
    threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);

    // connect horizontally oriented regions
    Mat connected;
    morphKernel = getStructuringElement(MORPH_RECT, Size(26, 13));
    morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);

    // find contours
    Mat mask = Mat::zeros(bw.size(), CV_8UC1);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));


    // filter contours
    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
    {
        Rect rect = boundingRect(contours[idx]);
        Mat maskROI(mask, rect);
        maskROI = Scalar(0, 0, 0);
        // fill the contour
        drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
        // ratio of non-zero pixels in the filled region
        double r = (double)countNonZero(maskROI)/(rect.width*rect.height);
		// assume at least 45% of the area is filled if it contains text & constraints on region size 
        if (r > 0.45 &&  (rect.height > 8 && rect.width > 8)){ 
			rectangle(rgb, rect, Scalar(0, 0, 0), 2);
	        int x_rect = (int) (rect.x * coefToPopplerX);
	        int y_rect = (int) (rect.y * coefToPopplerY);
    	    int height_rect = (int) (rect.height * coefToPopplerX);
        	int width_rect = (int) (rect.width * coefToPopplerY);

			// delete unnecessary elements
			double square_ratio = 0.4;		
			double row_ratio = 50;		
			double col_ratio = 3;		

			// delete squares	
			if ((((double) rect.height/rect.width > square_ratio) && 
						((double) rect.height/rect.width < 1/square_ratio)) || 
			    (((double) rect.height/rect.width < square_ratio) && 
						((double)rect.height/rect.width > 1/square_ratio)))  {
				make_white_rectangle (x_rect, y_rect, width_rect, height_rect);	

			// delete rows
			} else if ((double) rect.width/rect.height > row_ratio) {
				make_white_rectangle (x_rect, y_rect, width_rect, height_rect);	

			// delete columns
			} else if ((double) rect.height/rect.width > col_ratio) {
				make_white_rectangle (x_rect, y_rect, width_rect, height_rect);		
		
			} else {
		        Rectangle rect_text = Rectangle(x_rect, y_rect, width_rect, height_rect);
				areas.push_back(rect_text);
			}
        }
		imwrite(this->imagename, rgb);
    }
    return areas;
}

void PictureParser::make_white_rectangle (int x, int y, int width, int height){
	int white = 255;

	for (int x_ = x; x_ < x + width; x_++){
		for (int y_ = y; y_ < y + height; y_++){
			this->data[y_ * (this->width) * 4 + x_ * 4] = white;
	        this->data[y_ * (this->width) * 4 + x_ * 4 + 1] = white;
    	    this->data[y_ * (this->width) * 4 + x_ * 4 + 2] = white;
		}
	}
}

// scannig row by row
void PictureParser::select_title_rectangle(vector<Rectangle> & areas) {
    vector<PixInfo> black_rows = {};
    int white_rows_counter = 0;
    int y = 0;


    while ((y < this->height) && (white_rows_counter < 2000)) {
        int y_row = y;
        int x_row = -1;
        vector<int> heights = {};

        // scanning between 12% and 50% of the page width
        int w_start = int(this->width * 12 / 100);
        int w_end = int(this->width * 50 / 100);

        for (int x = w_start; x < w_end; x++) {
            if ((is_black(x, y)) && (y > 0) && (y < (int)(height/2))) {
                if (!is_black(x, y - 1)) {
                    int i = 1;
                    while (((y + i) < height) && (is_black(x, y + i))) {
                        i++;
                    }
                    if (x_row == -1) {
                        x_row = x;
                    }
                    if (white_background(x, y)) {
                        heights.push_back(i);

                    }
                }
            }
        }
        if (heights.size() > 0) {
            PixInfo pix_info = PixInfo(x_row, y_row, heights);
            black_rows.push_back(pix_info);
        } else {
            white_rows_counter++;
        }
        y++;
    }

    int max_height = 0;
    for (unsigned int j = 0; j < black_rows.size(); j++) {
        vector<int> *cur_height = black_rows[j].get_heights();
		sort (cur_height->begin(), cur_height->end());
      	if (cur_height->back() > max_height) {
            max_height = cur_height->back();
        }
    }

    // ============== find topmost possible title ==============
    max_height += 3; // height tolerance 5%
    max_height *= 0.88; // height tolerance 5%
	int min_x = 10000;
    int min_y = 10000;
    int top_margin = this->height*0.05; // top margin 5% to avoid headers

    for (unsigned int j = 0; j < black_rows.size(); j++) {
        vector<int> *cur_height = black_rows[j].get_heights();
        if (cur_height->back() >= max_height){
            int x = black_rows[j].get_x();
            int y = black_rows[j].get_y();
            if (x < min_x) {
                min_x = x;
            }
            if ((y < min_y) && (y > top_margin)) {
                min_y = y;
            }
        }
    }

    int max_y = min_y + max_height;

    // check whether the title is more than single-line
    // assumption that the number of lines is < 6 was made
    for (unsigned int j = 0; j < black_rows.size(); j++) {
        int next_y = black_rows[j].get_y();
        if (next_y > min_y + 6 * max_height) {
            break;
        }

        vector<int> *cur_height = black_rows[j].get_heights();
        if ((cur_height->back() >= max_height) && (next_y > max_y) 
								&& (next_y - max_y < 2 * max_height)) {
            max_y = next_y + max_height;
        }
    }


	// final title area obtained by row-by-row scanning
	this->title_x  = min_x - 140; // select title area only
    this->title_y = min_y - 0.25 * max_height;
    this->title_height = max_y - min_y + 0.6 * max_height;


	// ============== final title area correction ==============
	// modify obtained title_width using info from areas obtained by opencv
	
	stable_sort(areas.begin(), areas.end(), Rectangle::_smaller);

	int max_width = 0;
	double corr_ratio = 1.2;

    for (unsigned int i = 0; i < areas.size(); i++) {
		Rectangle rect = areas[i];
		if ((rect.get_y() >= title_y) && (rect.get_y() <= title_y+title_height)
		&& (rect.get_x() >= title_x)) {
			if (rect.get_width() + (rect.get_x()-title_x) > max_width) {
				max_width = rect.get_width() + (rect.get_x()-title_x);
			}
		}
	}
	if (max_width == 0) {
		this->title_width = this->width;
	} else {
		this->title_width = (int) max_width*corr_ratio;
	}
    return;
}

string PictureParser::parse_image() {
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, "eng")) {
        throw BiblioException("PictureParser: Could not initialize tesseract.\n");
    }

    Pix *image = pixRead(this->imagename.c_str());
    api->SetImage(image);

    char *out_text = api->GetUTF8Text();

    api->End();
    pixDestroy(&image);

    return out_text;
}

PictureParser::~PictureParser() {
    remove(imagename.c_str());
    remove(("_" + imagename).c_str());
}

