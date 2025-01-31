#pragma once

#include "Wog.h"

class LevelModel
{
public:

	LevelModel();
	virtual ~LevelModel();

	static void init();

private:
	static Wog* spWogInstance;

};

