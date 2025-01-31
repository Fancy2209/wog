#include "MovieFactory.h"
#include "Boy/Environment.h"

MovieFactory *MovieFactory::gInstance = NULL;

MovieFactory::MovieFactory() {};

MovieFactory::~MovieFactory() {};

MovieFactory* MovieFactory::instance()
{
	return gInstance;
};

void MovieFactory::init() { 
	gInstance = new MovieFactory();
};