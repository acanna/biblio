#include "RequesterManager.h"

RequesterManager::RequesterManager() {
	this->req = {};
	if (cfg.lookup("dblp.enabled")) {
	    req.push_back(get_requester("dblp"));
	}
	if (cfg.lookup("springer.enabled")) {
	    req.push_back(get_requester("springer"));
	}
	if (cfg.lookup("arxiv.enabled")) {
	    req.push_back(get_requester("arxiv"));
	}
	if (cfg.lookup("nature.enabled")) {
	    req.push_back(get_requester("nature"));
	}
	if (cfg.lookup("sciencedirect.enabled")) {
	    req.push_back(get_requester("scincedirect"));	}
	}
	if (cfg.lookup("scopus.enabled")) { 
	    req.push_back(get_requester("scopus"));
	}
}

RequesterManager::~RequesterManager(){
	for (unsigned int i = 0; i < req.size(); i++) {
		delete req[i];
	}
}

std::vector<Requester *> get_all_requesters(){
	return req;
}

static Requester * get_requester(std::string s){
	switch (s) {
		case "dblp": 
			if (cfg.lookup("dblp.enabled")){
	  			string url = cfg.lookup("dblp.url");
	  			return new DBLPRequester(url);
	    	} else {
				return nullptr;
			}
			break;
		case "springer":	
	  		if (cfg.lookup("springer.enabled")){
	  			string url = cfg.lookup("springer.url");
	  			string apikey = cfg.lookup("springer.apikey");
	  			return new SpringerRequester(url, apikey);
	    	} else {
				return nullptr;
			}
			break;
		case "arxiv":	
	  		if (cfg.lookup("arxiv.enabled")){
	  			string url = cfg.lookup("arxiv.url");
	  			return new ArxivRequester(url);
	    	} else {
				return nullptr;
			}
			break;
		case "nature":	
	  		if (cfg.lookup("nature.enabled")){
	  			string url = cfg.lookup("nature.url");
	  			return new NatureRequester(url);
	    	} else {
				return nullptr;
			}
			break;
		case "sciencedirect":	
	  		if (cfg.lookup("sciencedirect.enabled")){
	  			string url = cfg.lookup("sciencedirect.url");
	  			string apikey = cfg.lookup("sciencedirect.apikey");
	  			return new ScienceDirectRequester(url, apikey);
	    	} else {
				return nullptr;
			}
			break;
		case "scopus":	 
	  		if (cfg.lookup("scopus.enabled")){
	  			string url = cfg.lookup("scopus.url");
	  			string apikey = cfg.lookup("scopus.apikey");
	  			return new ScopusRequester(url, apikey);
	     	} else {
				return nullptr;
			}
	}
}