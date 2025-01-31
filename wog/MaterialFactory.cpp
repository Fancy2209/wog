#include "MaterialFactory.h"

MaterialFactory *MaterialFactory::gInstance = NULL;

MaterialFactory::MaterialFactory(const char * documentName) : TiXmlDocument(documentName)
{
	LoadFile(documentName, TIXML_ENCODING_UNKNOWN);
	// TODO
};

MaterialFactory::~MaterialFactory()
{

};

MaterialFactory* MaterialFactory::instance()
{
	return gInstance;
};

void MaterialFactory::init(const char * documentName) { 
	gInstance = new MaterialFactory(documentName);
};