#include "Requester.h"

using namespace std;

//EL эти кишки лучше спрятать вовнутрь класса Requester
//в функцию writeData последним параметров передавать указатель на Requester,
//тогда можно будет от этих глобальных переменных избавиться
char buffer[MAX_BUF + 1];
int bufferIndex;
int errorCode;
//эту функцию лучше сделать статической в классе Requester
size_t writeData(void *webBuffer, size_t size, size_t nmemb, void *userp) {
    size_t segSize = size * nmemb;
    if (bufferIndex + segSize > MAX_BUF) {
        *(int *) userp = 1;
        return 0;
    }
    memcpy((void *) &buffer[bufferIndex], webBuffer, segSize);
    bufferIndex += segSize;
    buffer[bufferIndex] = 0;
    return segSize;
}

void Requester::curl_init() {
    curl = curl_easy_init();
    if (!curl) {
        throw Biblio_exception("CURL initialization failed");
    }

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &errorCode);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
}

void Requester::curl_clean_up(){
	curl_easy_cleanup(curl);
}

char * Requester::curl_perform(string request) {
    errorCode = 0;
    bufferIndex = 0;

	curl_easy_setopt(curl, CURLOPT_URL, request.c_str());
    CURLcode result = curl_easy_perform(curl);
    
    if (result != CURLE_OK) {
        string what = "";
        switch (result) {
            case 1:
                what = "Unsupported protocol";
                break;
            case 2:
                what = "CURL initialization failed";
                break;
            case 3:
                what = "Wrong URL formatting";
                break;
            case 6:
                what = "Given remote host was not resolved";
                break;
            case 7:
                what = "Failed to connect to host or proxy";
                break;
            default:
                what = "Failed to perform query";
        }
        throw Biblio_exception(what);
    }
	
	return buffer;
}

Requester::Requester() {
    curl_init();
}

Requester::~Requester() {
    curl_clean_up();
}

vector<ArticleInfo> Requester::publication_request(const string &query) {
    string request = make_request(query);
	char * buffer = curl_perform(request);

    vector<ArticleInfo> articles;
    articles = parse_response(buffer);
    return articles;
}

