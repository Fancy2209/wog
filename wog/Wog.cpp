#include "Wog.h"

#include <assert.h>
#include <string>

#include "PlayerProfileFactory.h"
#include "LevelFactory.h"

#include "Boy/Environment.h"
#include "Boy/GamePad.h"
#include "Boy/Graphics.h"
#include "Boy/Mouse.h"
#include "Boy/ResourceManager.h"

using namespace Boy;

Wog *Wog::gInstance = NULL;

Wog::Wog()
{
	gInstance = this;
	BoyLib::Messenger::init();
	// mModel = new Model();
	// mController = new Controller();
	// mStatsAndAchivements = new StatsAndAchivements();
}

Wog::~Wog()
{
}

Wog *Wog::instance()
{
	if (gInstance == NULL)
	{
		gInstance = new Wog();
	}

	return gInstance;
}

void Wog::destroy()
{
	// destroy the singleton:
	assert(gInstance != NULL);
	delete gInstance;
	gInstance = NULL;
}

void Wog::preInitLoad()
{;
	Environment::instance()->getResourceManager()->parseResourceFile("properties/resources.xml", Environment::instance()->getCryptoKey());
	Environment::instance()->getResourceManager()->loadResourceGroup("bootstrap");
	Environment::instance()->getResourceManager()->loadResourceGroup("init");
	Environment::instance()->showSystemMouse(false);
}

void Wog::preInitdraw(Graphics *g)
{
	// Empty on purpose, game left it empty too
}

void Wog::init()
{
	Environment::instance()->sleep(500);
	PlayerProfileFactory::init();
	LevelFactory::init();
	/*
	SceneFactory::init();
	AnimationFactory::init();
	BallFactory::init();
	MovieFactory::init();
	IslandFactory::init();
	MaterialFactory::init("properties/materials.xml");
	EffectsFactory::init("properties/fx.xml");
	mRenderer = new WogRenderer();
	Environment::instance()->UNK1(0);
	GooBall::init();
  	LevelModel::init();
  	Particle::init();
	*/
}

void Wog::load()
{
	Environment::instance()->debugLog("loading game...\n");
	Environment::instance()->getResourceManager()->loadResourceGroup("common");
	Environment::instance()->debugLog("loading \'common\' complete.\n");
	// LevelFactory::instance()->loadLevel("MapWorldView", true);
	// Environment::instance()->debugLog("loading level complete.\n");
	// TODO: A bunch more stuff, need to make a vtable in ghidra for the Wog Class so it'll actually show the calls
	// Or it'll be pain to read the code

}

void Wog::loadComplete()
{
	// TODO
}

void Wog::update(float dt)
{
	// TODO

}

void Wog::draw(Graphics *g)
{
	// TODO

}

void Wog::windowResized(int oldWidth, int oldHeight, int newWidth, int newHeight)
{
	// TODO
}

void Wog::preShutdown()
{
	// TODO
}

void Wog::fullscreenToggled(bool isFullscreen)
{
	// TODO
}

void Wog::handleMouseAdded(int mouseId)
{
	// TODO
}

void Wog::handleMouseRemoved(int mousePadId)
{
	// TODO
}

void Wog::focusGained()
{
	// TODO
}

void Wog::focusLost()
{
	// TODO
}
