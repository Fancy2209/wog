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

void LevelFactory::loadLevel(std::string const& levelName, bool pauseTime)
{
	char *pBuffer;
	Boy::Environment::instance()->sprintf(pBuffer, 128, "res/levels/%s", levelName);
	Boy::Environment::instance()->debugLog("TODO: loadLevel(levelName: %s, pauseTime: %b)", levelName, pauseTime);
	// TODO
};