#include "LevelModel.h"
#include "Boy/Environment.h"

Wog *LevelModel::spWogInstance = NULL;

LevelModel::LevelModel() {};

LevelModel::~LevelModel() {};

void LevelModel::init() { 
	spWogInstance = Wog::instance();
};