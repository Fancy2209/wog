#pragma once

#include "Boy/Game.h"
#include "Boy/MouseListener.h"
#include "Boy/KeyboardListener.h"
#include "BoyLib/Messenger.h"

class Wog: public Boy::Game
{
public:

	Wog();
	virtual ~Wog();

	// singleton:
	static Wog *instance();
	static void destroy();

	// implementation of Game:
	virtual void preInitLoad();
	virtual void preInitdraw(Boy::Graphics* g);
	virtual void init();
	virtual void load();
	virtual void loadComplete();
	virtual void update(float dt);
	virtual void draw(Boy::Graphics *g);
	virtual void windowResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
	virtual void preShutdown();
	virtual void fullscreenToggled(bool isFullscreen);
	virtual void handleMouseAdded(int mouseId);
	virtual void handleMouseRemoved(int mousePadId);
	virtual void focusGained();
	virtual void focusLost();

private:
	static Wog *gInstance;
	//Model *mModel;
	//Controller *mController;
	//WogRenderer *mRenderer;
	WogRenderer *mRenderer;
	std::string mVersion;
	//StatsAndAchievements *mStatsAndAchivements;

};
