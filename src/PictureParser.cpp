#include "PictureParser.h"

using namespace std;

PictureParser::PictureParser(string filename, int xres, int yres, string imagename, string format, int dpi){
    this->filename = filename;  
    this->xres = xres;
    this->yres = yres;
    this->imagename = imagename;
    this->format = format;
    this->dpi = dpi;
}


void PictureParser::save_as_image (int page_num) {
 	poppler::document *doc = poppler::document::load_from_file(this->filename);
	const int pages_nbr = doc->pages();
    if (page_num >pages_nbr) {
    	throw runtime_error("PictureParser: Wrong page number indicated.\n");	
    }
	poppler::page* mypage =doc->create_page(page_num);

	poppler::page_renderer renderer;
	poppler::image myimage = renderer.render_page(mypage,this->xres, this->yres);	

	myimage.save(this->imagename, this->format, this->dpi);
    
    return;
}

char* PictureParser::parse_image() {
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	if (api->Init(NULL, "eng")) {
		throw runtime_error("PictureParser: Could not initialize tesseract.\n");	
	}

	Pix *image = pixRead(this->imagename.c_str());
	api->SetImage(image);

	char *out_text = api->GetUTF8Text();
	
	api->End();
	pixDestroy(&image);

//	delete [] outText;	
	return out_text;
}