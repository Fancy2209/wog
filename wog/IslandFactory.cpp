#include "IslandFactory.h"
#include "Boy/Environment.h"

IslandFactory *IslandFactory::gInstance = NULL;

IslandFactory::IslandFactory() {};

IslandFactory::~IslandFactory() {};

IslandFactory* IslandFactory::instance()
{
	return gInstance;
};

void IslandFactory::init() { 
	gInstance = new IslandFactory();
};