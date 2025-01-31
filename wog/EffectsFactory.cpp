#include "EffectsFactory.h"

EffectsFactory *EffectsFactory::gInstance = NULL;

EffectsFactory::EffectsFactory(const char * documentName) : TiXmlDocument(documentName)
{
	LoadFile(documentName, TIXML_ENCODING_UNKNOWN);
	// TODO
};

EffectsFactory::~EffectsFactory()
{

};

EffectsFactory* EffectsFactory::instance()
{
	return gInstance;
};

void EffectsFactory::init(const char * documentName) { 
	gInstance = new EffectsFactory(documentName);
};