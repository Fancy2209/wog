#include "SceneFactory.h"
#include "Boy/Environment.h"

SceneFactory *SceneFactory::gInstance = NULL;

SceneFactory::SceneFactory() {};

SceneFactory::~SceneFactory() {};

SceneFactory* SceneFactory::instance()
{
	return gInstance;
};

void SceneFactory::init() { 
	gInstance = new SceneFactory();
};