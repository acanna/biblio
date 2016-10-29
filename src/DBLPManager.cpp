#include "DBLPManager.h"
#include "Biblio_exception.h"

using namespace std;

char buffer[MAX_BUF + 1];
int bufferIndex;


size_t writeData (void * webBuffer, size_t size, size_t nmemb, void * userp) {
	size_t segSize = size * nmemb;
	if (bufferIndex + segSize > MAX_BUF) {
		*(int *)userp = 1;
		return 0;
	}
	memcpy ( (void*)& buffer[bufferIndex], webBuffer, segSize);
	bufferIndex += segSize;
	buffer[bufferIndex] = 0; 
	return segSize;
}

DBLPManager::DBLPManager() {
	curl = curl_easy_init();
	if (!curl) {
		throw Biblio_exception("DBLP: CURL initialization failed");
	}	
	
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)& errorCode);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
}

DBLPManager::~DBLPManager() {
	curl_easy_cleanup(curl);
}

vector <ArticleInfo> DBLPManager::publicationRequest(const string & query) {
	CURLcode result;
	errorCode = 0;
	bufferIndex = 0;

	string request = URL + query;
    
	curl_easy_setopt(curl, CURLOPT_URL, request.c_str());		
	
	result = curl_easy_perform(curl);

	vector <ArticleInfo> articles; 	

	if (result != CURLE_OK) {
		string what = "";
		switch(result){
			case 1:
				what = "DBLP: Unsupported protocol";
				break;
			case 2:
				what = "DBLP: CURL initialization failed"; 
				break;
			case 3:
				what = "DBLP: Wrong URL formatting";
				break;
			case 6:
				what = "DBLP: Given remote host was not resolved";
				break;
			case 7:
				what = "DBLP: Failed to connect to host or proxy";
				break;
			default:
				what = "DBLP: Failed to perform query";
		}			
		throw Biblio_exception(what);
	}
	articles = parseResponse();
	return articles;
}

vector <ArticleInfo> DBLPManager::parseResponse() {
	Json::Reader reader;
	Json::Value root;

	bool parsingSuccessful = reader.parse(buffer, root);
	vector <ArticleInfo> articles; 	
	 
	if (!parsingSuccessful) {
	        string what = "DBLP: Parser error: " + reader.getFormattedErrorMessages();
		throw Biblio_exception(what);
	}
	
	Json::Value hits = root["result"]["hits"]["hit"];

	for (unsigned int i = 0; i < hits.size(); i++) {
		Json::Value info = hits[i]["info"];		
		articles.push_back(ArticleInfo(info));
	}
	return articles;

}


