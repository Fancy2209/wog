#include "AnimationFactory.h"
#include "Boy/Environment.h"

AnimationFactory *AnimationFactory::gInstance = NULL;

AnimationFactory::AnimationFactory() {};

AnimationFactory::~AnimationFactory() {};

AnimationFactory* AnimationFactory::instance()
{
	return gInstance;
};

void AnimationFactory::init() { 
	gInstance = new AnimationFactory();
};