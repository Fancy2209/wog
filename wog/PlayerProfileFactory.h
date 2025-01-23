#pragma once


class PlayerProfileFactory
{
public:

	PlayerProfileFactory();
	virtual ~PlayerProfileFactory();

	static void init();
	static PlayerProfileFactory* instance();

private:
	static PlayerProfileFactory* gInstance;

};

