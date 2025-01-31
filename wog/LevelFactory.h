#pragma once


class LevelFactory
{
public:

	LevelFactory();
	virtual ~LevelFactory();

	static void init();
	static LevelFactory* instance();

	virtual void loadLevel(std::string const& levelName, bool pauseTime);

private:
	static LevelFactory* gInstance;

};

