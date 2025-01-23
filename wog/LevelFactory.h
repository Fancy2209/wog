#pragma once


class LevelFactory
{
public:

	LevelFactory();
	virtual ~LevelFactory();

	static void init();
	static LevelFactory* instance();

private:
	static LevelFactory* gInstance;

};

