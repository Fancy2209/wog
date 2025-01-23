#include "PlayerProfileFactory.h"
#include "Boy/Environment.h"

PlayerProfileFactory *PlayerProfileFactory::gInstance = NULL;


PlayerProfileFactory::PlayerProfileFactory() {};

PlayerProfileFactory::~PlayerProfileFactory() {};

PlayerProfileFactory* PlayerProfileFactory::instance()
{
	return gInstance;
};

void PlayerProfileFactory::init() { 
	gInstance = new PlayerProfileFactory();
};