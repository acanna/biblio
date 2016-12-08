#pragma once

#include "Requesters/Requester.h"

class RequesterManager {
private:
	std::vector<Requester *> req;
	
public:
	RequesterManager();
	~RequesterManager();
	std::vector<Requester *> get_all_requesters();
	static Requester * get_requester(std::string s);
};