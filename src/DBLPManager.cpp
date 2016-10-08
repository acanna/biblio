#include "DBLPManager.h"

char buffer[MAX_BUF + 1];
int bufferIndex;


size_t writeData (void * webBuffer, size_t size, size_t nmemb, void *userp) {
	int segSize = size * nmemb;
	if (bufferIndex + segSize > MAX_BUF) {
		*(int *)userp = 1;
		return 0;
	}
	memcpy ( (void*)& buffer[bufferIndex], webBuffer, (size_t) segSize);
	bufferIndex += segSize;
	buffer[bufferIndex] = 0; 
	return segSize;
}

DBLPManager::DBLPManager(){
	curl = curl_easy_init();
	if (!curl) {
		std::cout << "Error by CURL initialization \n"; 
		return;
	}	

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)& errorCode);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
//	std::cout << "Initialization successfully completed \n";	

}

DBLPManager::~DBLPManager(){
	curl_easy_cleanup(curl);
}

vector <ArticleInfo> DBLPManager::publicationRequest(string query){
	CURLcode result;
	errorCode = 0;
	bufferIndex = 0;

	string request = URL + query;
	std::cout << "Request:" << request <<"\n";
	curl_easy_setopt(curl, CURLOPT_URL, request.c_str());

	result = curl_easy_perform(curl);

//	std::cout << result << " "<< errorCode << "\n";			
	
	vector <ArticleInfo> articles; 	
	if (result == 0) {
		articles = parseResponse();
	}
	return articles;

}

vector <ArticleInfo> DBLPManager::parseResponse() {
	Json::Reader reader;
	Json::Value root;

	bool parsingSuccessful = reader.parse(buffer, root);
	vector <ArticleInfo> articles; 	

	if (!parsingSuccessful) {
	        std::cout  << "Failed to parse " << reader.getFormattedErrorMessages() <<"\n";
	        return articles;
	}
	
	Json::Value hits = root["result"]["hits"]["hit"];

	for (unsigned int i = 0; i < hits.size(); i++) {
		Json::Value info = hits[i]["info"];
		articles.push_back(ArticleInfo(info));

	}
	return articles;

}


