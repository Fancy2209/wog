#include "BallFactory.h"
#include "Boy/Environment.h"

BallFactory *BallFactory::gInstance = NULL;

BallFactory::BallFactory() {};

BallFactory::~BallFactory() {};

BallFactory* BallFactory::instance()
{
	return gInstance;
};

void BallFactory::init() { 
	gInstance = new BallFactory();
	gSoundMap = {
		{"throw", 0},
		{"drop", 1},
		{"pickup", 2},
		{"marker", 3},
		{"attach", 4},
		{"attachcloser", 5},
		{"detaching", 6},
		{"detached", 7},
		{"snap", 8},
		{"death", 9},
		{"suction", 10},
		{"ignite", 11},
		{"extinguish", 12},
		{"detonate", 13},
		{"collidegeom", 14},
		{"collidesame", 15},
		{"collidediff", 16}
	};
};