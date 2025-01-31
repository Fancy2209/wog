#include "Wog.h"

#include <assert.h>
#include <string>

#include "Model.h"
#include "Controller.h"

#include "PlayerProfileFactory.h"
#include "LevelFactory.h"
#include "SceneFactory.h"
#include "AnimationFactory.h"
#include "BallFactory.h"
#include "MovieFactory.h"
#include "IslandFactory.h"
#include "MaterialFactory.h"
#include "EffectsFactory.h"
#include "WogRenderer.h"
#include "GooBall.h"
#include "LevelModel.h"
#include "Particle.h"

#include "Boy/Environment.h"
#include "Boy/GamePad.h"
#include "Boy/Graphics.h"
#include "Boy/Mouse.h"
#include "Boy/ResourceManager.h"

Wog *Wog::gInstance = NULL;

Wog::Wog()
{
	gInstance = this;
	BoyLib::Messenger::init();
	mModel = new Model();
	mController = new Controller();
	//mStatsAndAchivements = new StatsAndAchivements();
}

Wog::~Wog()
{
	// TODO
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
{
	Boy::Environment::instance()->disableFullScreenToggle();
	Boy::Environment::instance()->getResourceManager()->parseResourceFile("properties/resources.xml", Boy::Environment::instance()->getCryptoKey());
	Boy::Environment::instance()->getResourceManager()->loadResourceGroup("bootstrap");
	Boy::Environment::instance()->getResourceManager()->loadResourceGroup("init");
	Boy::Environment::instance()->showSystemMouse(false);
}

void Wog::preInitdraw(Boy::Graphics *g)
{
	// Empty on purpose, game left it empty too
}

void Wog::init()
{
	Boy::Environment::instance()->sleep(500);
	PlayerProfileFactory::init();
	LevelFactory::init();
	SceneFactory::init();
	AnimationFactory::init();
	BallFactory::init();
	MovieFactory::init();
	IslandFactory::init();
	MaterialFactory::init("properties/materials.xml");
	EffectsFactory::init("properties/fx.xml");
	mWogRenderer = new WogRenderer();
	Boy::Environment::instance()->getGraphics()->setClearZ(0);
	GooBall::init();
  	LevelModel::init();
  	Particle::init();
}

void Wog::load()
{
	Boy::Environment::instance()->debugLog("loading game...\n");
	Boy::Environment::instance()->getResourceManager()->loadResourceGroup("common");
	Boy::Environment::instance()->debugLog("loading \'common\' complete.\n");
	LevelFactory::instance()->loadLevel("MapWorldView", true);
	Boy::Environment::instance()->debugLog("loading level complete.\n");
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

void Wog::draw(Boy::Graphics *g)
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
