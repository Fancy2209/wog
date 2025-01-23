#include "LevelFactory.h"
#include "Boy/Environment.h"

LevelFactory *LevelFactory::gInstance = NULL;

LevelFactory::LevelFactory() {};

LevelFactory::~LevelFactory() {};

LevelFactory* LevelFactory::instance()
{
	return gInstance;
};

void LevelFactory::init() { 
	gInstance = new LevelFactory();
};