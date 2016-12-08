#pragma once

#include "Requesters/Requester.h"
#include "Config.h"

class RequesterManager {
private:
	std::vector<Requester *> req;
	
public:
	RequesterManager();
	~RequesterManager();
	std::vector<Requester *> get_all_requesters();
};