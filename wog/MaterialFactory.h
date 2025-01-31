#pragma once
#include "tinyxml.h"

class MaterialFactory: public TiXmlDocument
{
public:
	MaterialFactory(const char * documentName);
	virtual ~MaterialFactory();

	static void init(const char * documentName);
	static MaterialFactory* instance();

private:
	static MaterialFactory* gInstance;

};

