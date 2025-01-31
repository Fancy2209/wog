#pragma once
#include "tinyxml.h"

class EffectsFactory: public TiXmlDocument
{
public:
	EffectsFactory(const char * documentName);
	virtual ~EffectsFactory();

	static void init(const char * documentName);
	static EffectsFactory* instance();

private:
	static EffectsFactory* gInstance;

};

