#pragma once

#include <map>
#include <string>
#include "GooController.h"
#include "BoyLib/Tickable.h"
#include "BoyLib/TickableQueue.h"
#include "BoyLib/TickableSet.h"
#include "BoyLib/MessageListener.h"

class Model: public BoyLib::Tickable, BoyLib::MessageListener/*, IslandModel*/, BoyLib::TickableQueue, BoyLib::TickableSet
{
public:

	Model();
	virtual ~Model();
	virtual bool tick();
	virtual void cancel();
	virtual void handleMessage(const std::string &messageId, BoyLib::MessageSource *source, std::map<std::string,std::string> *params);
};

