#include "PictureParser.h"


using namespace std;

PictureParser::PictureParser(string filename, int xres, int yres, 
							 string imagename, string format, int dpi){

    this->filename = filename;  
    this->xres = xres;
    this->yres = yres;
    this->imagename = imagename;
    this->format = format;
    this->dpi = dpi;
	this->data = nullptr;
	this->title_x = 0;
	this->title_y = 0;
	this->title_height = 0;
	this->title = "";
}

string PictureParser::get_title() {
	return this->title;
}

PixInfo::PixInfo(int const & x, int const & y, std::vector <int> const & row_height){
	this-> x = x;
	this-> y = y;
	this->row_height = row_height;
}

vector<int> * PixInfo::get_height() {
	return & this->row_height;
}

int PixInfo::get_x() {
	return this->x;
}

int PixInfo::get_y() {
	return this->y;
}

void PictureParser::save_as_image() {
    int page_num = 0;
 	poppler::document *doc = poppler::document::load_from_file(this->filename);
	const int pages_nbr = doc->pages();
    if (page_num > pages_nbr) {
    	throw Biblio_exception("PictureParser: Wrong page number indicated.\n");
    }
	poppler::page* mypage = doc->create_page(page_num);

	this->width = int(mypage->page_rect().width()*(this->xres)/72);
	this->height = int(mypage->page_rect().height()*(this->yres)/72);
	
	poppler::page_renderer renderer;

	poppler::image cur_image = renderer.render_page(mypage, this->xres, 
				this->yres, this->title_x, this->title_y, this->width, this->height);	
	this->data = cur_image.data();
    
	select_title_rectangle();

	cur_image = renderer.render_page(mypage, this->xres, 
				this->yres, this->title_x, this->title_y, this->width, this->title_height);	
	cur_image.save(this->imagename, this->format, this->dpi);

    parse_image();
	
    return;
}

bool PictureParser::is_black(int x, int y) {
	int black = 60; // (color <= black) <=> (black pix)
					 // (color > black) <=> (white pix)

	//	avoid other colors
	if  ((((this->data[y * (this->width) * 4 + x * 4]) & 255) > black) ||
	    (((this->data[y * (this->width) * 4 + x * 4 + 1]) & 255) > black) ||
		(((this->data[y * (this->width) * 4 + x * 4 + 2]) & 255) > black)) return false;

	return (((int)(this->data[y * (this->width) * 4 + x * 4]) & 255) * 
			((int)(this->data[y * (this->width) * 4 + x * 4 + 3]) & 255) / 255 <= black);
}

void PictureParser::select_title_rectangle() {
    ofstream out("image_bytes.txt");
	vector<PixInfo> black_rows = {};
	int whight_rows_counter = 0;
	int y = 0;


	while ((y < this->height) && (whight_rows_counter < 2000)) {
		int y_row = y;
		int x_row = -1;
		vector <int >heights = {};
        
        // scanning between 1/4 and 1/2 of the page width  
        int w_start = int(this->width/4);  
        int w_end = int(this->width/2);     
        
		for (int x = w_start; x < w_end; x++) {
			if ((is_black(x,y)) && (y > 0) && (y < height)) {
				if (is_black(x, y-1)) {
					int i = 1;
					while (((y+i) < height) && (is_black(x, y+i))) {
						i++;
					}
					if (x_row == -1) {
						x_row = x;
					}
					heights.push_back(i);	
				}
			}			
		}
		if (heights.size() > 0) {
			PixInfo pix_info = PixInfo(x_row, y_row, heights); 
			black_rows.push_back(pix_info);			
		} else {
			whight_rows_counter++;
		}
		y++;
	}

	int max_height = 0;
	for (unsigned int j = 0; j < black_rows.size(); j++) {
		vector<int> * cur_height = black_rows[j].get_height();
		sort (cur_height->begin(), cur_height->end());
		if (cur_height->back() > max_height) {
			max_height = cur_height->back();
		}
	}
		
	max_height -= 5; // height tolerance -5 pix
	int min_x = 10000;
	int min_y = 10000;
	for (unsigned int j = 0; j < black_rows.size(); j++) {
		vector<int> * cur_height = black_rows[j].get_height();
		if (cur_height->back() >= max_height) {
			if (black_rows[j].get_x() < min_x){
				min_x = black_rows[j].get_x();
			}

			if (black_rows[j].get_y() < min_y){
				min_y = black_rows[j].get_y();
			}
		}
	}

	//this->title_x = min_x - 25; // select title area only 
    this->title_x = 0; // select across the width of the page 
	this->title_y = min_y - 0.25*max_height;
	
    this->title_height = max_height + 0.75*max_height;
	return;

}

void PictureParser::parse_image() {
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	if (api->Init(NULL, "eng")) {
		throw Biblio_exception("PictureParser: Could not initialize tesseract.\n");
	}

	Pix *image = pixRead(this->imagename.c_str());
	api->SetImage(image);

	char *out_text = api->GetUTF8Text();
	
	api->End();
	pixDestroy(&image);

	this->title = string(out_text);
	cout << out_text;
	return;
}
