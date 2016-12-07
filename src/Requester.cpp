#include "Requester.h"

using namespace std;


size_t Requester::writeData(void *webBuffer, size_t size, size_t nmemb, void *userp) {

//EL эти кишки лучше спрятать вовнутрь класса Requester
//в функцию writeData последним параметров передавать указатель на Requester,
//тогда можно будет от этих глобальных переменных избавиться

//эту функцию лучше сделать статической в классе Requester
    size_t segSize = size * nmemb;
    int bufferIndex = (*(Requester *) userp).bufferIndex;
    if (bufferIndex + segSize > MAX_BUF) {
        (*(Requester *) userp).errorCode = 1;
        return 0;
    }
    memcpy((void *) &(*(Requester *) userp).buffer[bufferIndex], webBuffer, segSize);
    bufferIndex += segSize;
    (*(Requester *) userp).bufferIndex += segSize;
    (*(Requester *) userp).buffer[bufferIndex] = 0;
    return segSize;
}

void Requester::curl_init() {
    curl = curl_easy_init();
    if (!curl) {
        throw BiblioException("CURL initialization failed");
    }

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) this);
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
        throw BiblioException(what);
    }
	
	return buffer;
}

Requester::Requester() {
    curl_init();
    bufferIndex = 0;
    errorCode = 0;
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

Requester::Requester(const Requester &r) {
    curl_init();
    bufferIndex = r.bufferIndex;
    errorCode = r.errorCode;
}

