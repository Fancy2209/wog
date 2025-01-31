#pragma once


class MovieFactory
{
public:

	MovieFactory();
	virtual ~MovieFactory();

	static void init();
	static MovieFactory* instance();

private:
	static MovieFactory* gInstance;

};

