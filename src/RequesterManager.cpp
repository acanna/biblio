#include "RequesterManager.h"
#include "Requesters/DBLPRequester.h"
#include "Requesters/SpringerRequester.h"
#include "Requesters/ArxivRequester.h"
#include "Requesters/NatureRequester.h"
#include "Requesters/ScienceDirectRequester.h"
#include "Requesters/ScopusRequester.h"

using namespace std;

RequesterManager::RequesterManager() {
	req = {};
	if (Config::get_instance().lookup("dblp.enabled")) {
		string url = Config::get_instance().lookup("dblp.url");
	    req.push_back(new DBLPRequester(url));
	}
	if (Config::get_instance().lookup("springer.enabled")) {
		string url = Config::get_instance().lookup("springer.url");
		string apikey = Config::get_instance().lookup("springer.apikey");
	    req.push_back(new SpringerRequester(url, apikey));
	}
	if (Config::get_instance().lookup("arxiv.enabled")) {
		string url = Config::get_instance().lookup("arxiv.url");
	    req.push_back(new ArxivRequester(url));
	}
	if (Config::get_instance().lookup("nature.enabled")) {
		string url = Config::get_instance().lookup("nature.url");
	    req.push_back(new NatureRequester(url));
	}
	if (Config::get_instance().lookup("sciencedirect.enabled")) {
		string url = Config::get_instance().lookup("sciencedirect.url");
		string apikey = Config::get_instance().lookup("sciencedirect.apikey");
	    req.push_back(new ScienceDirectRequester(url, apikey));
	}
	if (Config::get_instance().lookup("scopus.enabled")) {
		string url = Config::get_instance().lookup("scopus.url");
		string apikey = Config::get_instance().lookup("scopus.apikey");
	    req.push_back(new ScopusRequester(url, apikey));
	}
}

RequesterManager::~RequesterManager(){
	for (unsigned int i = 0; i < req.size(); i++) {
		delete req[i];
	}
}

std::vector<Requester *> RequesterManager::get_all_requesters(){
	return req;
}

