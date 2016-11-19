#include "PictureParser.h"


using namespace std;
/*
TitleRect::TitleRect(int const title_x, int const title_y, int const title_height){
	this->title_x = title_x;
	this->title_y = title_y;
	this->title_height = title_height;
}
int const &TitleRect::get_x(){
	return this->title_x;
}
int const &TitleRect::get_y(){
	return this->title_y;
}
int const &TitleRect::get_height(){
	return this->title_height;
}
int const & TitleRect::get_rate(){
	return this->rate;
}
void TitleRect::set_rate(int rate){
	this->rate = rate;
}*/
/*
PixInfo::PixInfo(int const x, int const y, int const row_height) {
    this->x = x;
    this->y = y;
    this->row_height = row_height;
}
int PixInfo::get_height() const {
    return this->row_height;
}
bool PixInfo::is_greater(const PixInfo &pi_1, const PixInfo &pi_2) {
	if (pi_1.get_height() != pi_2.get_height()) {
    	return pi_1.get_height() > pi_2.get_height();
	} else {
		return pi_1.get_x() < pi_2.get_x();
	}
}
*/

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

//PictureParser::PictureParser() {}

PictureParser::PictureParser(string const &filename, int const xres, int const yres,
                             string const &imagename, string const &format, int const dpi) {

    this->filename = filename;
    this->xres = xres;
    this->yres = yres;
    this->imagename = imagename;
    this->format = format;
    this->dpi = dpi;
    this->data = nullptr;
    //this->title_rect = {};
    //this->titles = {};
    this->title = "";
}

string const &PictureParser::get_title() {
    return this->title;
}

void PictureParser::find_title() {
    int screen_res = 72;
    int page_num = 0;
    string result = "";

    poppler::document *doc = poppler::document::load_from_file(this->filename);
    const int pages_nbr = doc->pages();
    if (page_num > pages_nbr) {
        throw Biblio_exception("PictureParser: Wrong page number indicated.\n");
    }
    poppler::page *mypage = doc->create_page(page_num);

    this->width = int(mypage->page_rect().width() * (this->xres) / screen_res);
    this->height = int(mypage->page_rect().height() * (this->yres) / screen_res);

    poppler::page_renderer renderer;
    poppler::image cur_image = renderer.render_page(mypage,
                                                    this->xres, this->yres, 0, 0, this->width, this->height);
    this->data = cur_image.data();

    vector<poppler::font_info> fonts = doc->fonts();
    select_title_rectangle();

    // pdf as image
    if (fonts.size() == 0) {
        poppler::image rect_image = renderer.render_page(mypage,
                                                         this->xres, this->yres, this->title_x,                                             		      		      this->title_y, this->width, this->title_height);
        rect_image.save(this->imagename, this->format, this->dpi);
        result = parse_image();

        // pdf as text
    } else {
        double x_rect = this->title_x * screen_res / this->xres;
        double y_rect = this->title_y * screen_res / this->yres;
        double height_rect = this->title_height * screen_res / this->yres;
        double width_rect = this->width * screen_res / this->xres;
        poppler::rectangle<double> rect = poppler::rectangle<double>(x_rect, y_rect, width_rect, height_rect);
        result = mypage->text(rect).to_latin1();
        result = raw_to_formatted(result);
    }
    this->title = result;
}

bool PictureParser::is_black(int x, int y) {
    int black = 60; // (color <= black) <=> (black pix)
    // (color > black) <=> (white pix)

    //	avoid other colors
    if ((((this->data[y * (this->width) * 4 + x * 4]) & 255) > black) ||
        (((this->data[y * (this->width) * 4 + x * 4 + 1]) & 255) > black) ||
        (((this->data[y * (this->width) * 4 + x * 4 + 2]) & 255) > black))
        return false;

    return (((int) (this->data[y * (this->width) * 4 + x * 4]) & 255) *
            ((int) (this->data[y * (this->width) * 4 + x * 4 + 3]) & 255) / 255 <= black);
}

bool PictureParser::is_white(int x, int y) {
    int white = 230; // (color >= white) <=> (white pix)
    // (color < white) <=> (black pix)

    return (((int) (this->data[y * (this->width) * 4 + x * 4]) & 255) *
            ((int) (this->data[y * (this->width) * 4 + x * 4 + 3]) & 255) / 255 >= white);
}

bool PictureParser::is_dark(int x, int y) {
    int black = 127; // (color <= black) <=> (black pix)
    // (color > black) <=> (white pix)
    return (((int) (this->data[y * (this->width) * 4 + x * 4]) & 255) *
            ((int) (this->data[y * (this->width) * 4 + x * 4 + 3]) & 255) / 255 <= black);
}

int PictureParser::check_b_w_pix_ratio(int title_x, int title_y, int title_height) {
    int black = 0;
    int dark = 0;
    int white = 0;
    for (int x = title_x; x < int(this->width/2); x++){
        for (int y = title_y; y < (title_y + title_height); y++){
            if (is_black(x, y)) {
                black++;
            } else if (is_dark(x, y)) {
                dark++;
            } else {
                white++;
            }
        }
    }
    cout << "BLACK : DARK : WHITE  " << black << " : " << dark << " : " << white << endl;
    return 0;
}

bool PictureParser::white_background(int x_start, int y) {
    int white_counter = 0;
    for (int x = x_start; x < (int)(this->width / 2); x++){
        if (is_white(x, y)) {
            white_counter++;
        }
        if (white_counter > 10) {
            return true;
        }
    }
    return false;
}

// scannig row by row
void PictureParser::select_title_rectangle() {
    ofstream out("image_bytes.txt");
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


    float ratio = 9.7; // max_height <= ratio * average_height
    // to avoid parsing images
    long average_height = 0;
    for (unsigned int j = 0; j < black_rows.size(); j++) {
        vector<int> *cur_height = black_rows[j].get_heights();
        sort(cur_height->begin(), cur_height->end());
        average_height += cur_height->back();
    }
    average_height = average_height / black_rows.size();
    out << average_height << endl;
    out << endl;


    // statistics on heights
    for (unsigned int j = 0; j < black_rows.size(); j++) {
        vector<int> * cur_height = black_rows[j].get_heights();
        //sort (cur_height->begin(), cur_height->end());
        out << "-----------------------" << endl;
        out << 	"x " << black_rows[j].get_x() << "\t y " <<  black_rows[j].get_y() << endl;
        out << "min height "<< cur_height->front() << "\t max height"<< cur_height->back() << endl;
    }
    out.close();



    int max_height = 0;
    for (unsigned int j = 0; j < black_rows.size(); j++) {
        vector<int> *cur_height = black_rows[j].get_heights();
        //sort(cur_height->begin(), cur_height->end());
        while ((cur_height->size() > 0) && (cur_height->back() > (int)(average_height*ratio))){
            cur_height->pop_back();
        }
        if (cur_height->back() > max_height) {
            max_height = cur_height->back();
        }
    }

    // ============== find topmost possible title ==============
    //max_height -= 5; // height tolerance -5 pix
    max_height *= 0.95; // height tolerance 5%
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
        if ((cur_height->back() >= max_height) && (next_y > max_y) && (next_y - max_y < 2 * max_height)) {
            max_y = next_y + max_height;
        }
    }


    //this->title_x = 0; // select across the width of the page
    this->title_x = min_x - 140; // select title area only

    this->title_y = min_y - 0.25 * max_height;
    this->title_height = max_y - min_y + 0.75 * max_height;
    return;
}

// scanning pix by pix
/*void PictureParser::select_title_rectangle() {
    ofstream out("image_bytes.txt");
    vector<PixInfo> black_rows = {};
    int y = 0;
    while (y < this->height) {
        // scanning between 12% and 50% of the page width
        int w_start = int(this->width * 12 / 100);
        int w_end = int(this->width * 50 / 100);
        for (int x = w_start; x < w_end; x++) {
           if ((is_black(x, y)) && (y > 0) && (y < (int)(this->height/2))) {
                if (!is_black(x, y - 1)) {
                    int i = 1;
                    while (((y + i) < height) && (is_black(x, y + i))) {
                        i++;
                    }
                    if (white_background(x, y)) {
						PixInfo pix_info = PixInfo(x, y, i);
		            	black_rows.push_back(pix_info);
                    }
                }
            }
        }
        y++;
    }
	stable_sort(black_rows.begin(), black_rows.end(), PixInfo::is_greater);
	int ratio = 10; // max_height <= ratio * average_height
				     // to avoid parsing images
	long average_height = 0;
    for (unsigned int j = 0; j < black_rows.size(); j++) {
		average_height += black_rows[j].get_height();
    }
    average_height = (long) (average_height / black_rows.size());
	out << average_height << endl;
	out << endl;

	 // statistics on heights
      for (unsigned int j = 0; j < black_rows.size(); j++) {
          vector<int> * cur_height = black_rows[j].get_heights();
          //sort (cur_height->begin(), cur_height->end());
          out << "-----------------------" << endl;
          out << 	"x " << black_rows[j].get_x() << "\t y " <<  black_rows[j].get_y() << endl;
          out << "min height "<< cur_height->front() << "\t max height"<< cur_height->back() << endl;
      }
      out.close();
    int max_height = 0;
	int top_margin = this->height*0.05; // top margin 5% to avoid headers
	int half_page = this->height*0.5;
    for (unsigned int j = 0; j < black_rows.size(); j++) {
        if ((black_rows[j].get_x() < half_page) && (black_rows[j].get_x() > top_margin)
							&& (black_rows[j].get_height() < (int)(average_height*ratio))) {
        	max_height = black_rows[j].get_height();
			break;
		}
    }
	// ============== find topmost possible title ==============
    //max_height -= 5; // height tolerance -5 pix
    max_height *= 0.95; // height tolerance 5%
    int min_x = 10000;
    int min_y = 10000;

    for (unsigned int j = 0; j < black_rows.size(); j++) {
        if (black_rows[j].get_height() >= max_height){
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
        if ((black_rows[j].get_height() >= max_height) && (next_y > max_y) && (next_y - max_y < 2 * max_height)) {
            max_y = next_y + max_height;
        }
    }
    //this->title_x = 0; // select across the width of the page
    this->title_x = min_x - 140; // select title area only

    this->title_y = min_y - 0.25 * max_height;
    this->title_height = max_y - min_y + 0.75 * max_height;
    return;
}*/

string PictureParser::parse_image() {
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, "eng")) {
        throw Biblio_exception("PictureParser: Could not initialize tesseract.\n");
    }

    Pix *image = pixRead(this->imagename.c_str());
    api->SetImage(image);

    char *out_text = api->GetUTF8Text();

    api->End();
    pixDestroy(&image);

    return out_text;
}
