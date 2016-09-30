#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <fstream> 
using namespace std;

#define MAX_BUF 65536

char buffer[MAX_BUF + 1];
int bufferIndex;

/* 
 * Запись данных в буфер. Возвращает 0, если запись не удалась.
 */
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

int main (void) {
	CURL *curl;
	CURLcode result;
	int errorCode = 0;
	bufferIndex = 0;
	string URL;
	//URL = "www.dblp.uni-trier.de";
	URL = "http://dblp.org/search/publ/api?q=Linsky&format=json";

	/*init curl*/
	curl = curl_easy_init();
	if (!curl) {
		cout << "Error by CURL initialization"; 
		return 0;
	}	
	
	curl_easy_setopt(curl, CURLOPT_URL, "http://dblp.org/search/publ/api?q=Linsky&format=json");
	//curl_easy_setopt(curl, CURLOPT_URL, URL); 	
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)& errorCode);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
	
	result = curl_easy_perform(curl);
	cout << result, errorCode;

	if (result == 0) {
		ofstream output("output.txt") ;
		output << buffer;
	}

	curl_easy_cleanup(curl);
	//output.close();
  	return 0;
}

