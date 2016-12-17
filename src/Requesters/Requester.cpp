#include "Requester.h"

using namespace std;


size_t Requester::writeData(void *webBuffer, const size_t size, const size_t nmemb, void *userp) {
    Requester *req = (Requester*)userp;
    size_t segSize = size * nmemb;
    int bufferIndex = req->bufferIndex;
    if (bufferIndex + segSize > MAX_BUF) {
        req->errorCode = 1;
        return 0;
    }
    memcpy((void *) &(req->buffer[bufferIndex]), webBuffer, segSize);
    bufferIndex += segSize;
    req->bufferIndex += segSize;
    req->buffer[bufferIndex] = 0;
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

void Requester::curl_perform(const string &request) {
    errorCode = 0;
    bufferIndex = 0;
    for (unsigned int i = 0; i < MAX_BUF; i++) {
        buffer[i] = 0; 
    }

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
    curl_perform(request);
    return parse_response();
}

Requester::Requester(const Requester &r) {
    curl_init();
    bufferIndex = r.bufferIndex;
    errorCode = r.errorCode;
}

