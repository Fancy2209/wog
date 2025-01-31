#pragma once

#include <map>
#include <string>
#include "GooController.h"
#include "BoyLib/MessageListener.h"

class Controller/*: public GooController, BoyLib::MessageListener, LevelController, IslandController, WorldController*/
{
public:

	Controller();
	virtual ~Controller();
	virtual void handleMessage(const std::string &messageId, BoyLib::MessageSource *source, std::map<std::string,std::string> *params);
};

