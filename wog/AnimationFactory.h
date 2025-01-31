#pragma once


class AnimationFactory
{
public:

	AnimationFactory();
	virtual ~AnimationFactory();

	static void init();
	static AnimationFactory* instance();

private:
	static AnimationFactory* gInstance;

};

