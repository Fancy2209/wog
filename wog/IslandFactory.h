#pragma once


class IslandFactory
{
public:

	IslandFactory();
	virtual ~IslandFactory();

	static void init();
	static IslandFactory* instance();

private:
	static IslandFactory* gInstance;

};

