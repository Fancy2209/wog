#pragma once

#include "Wog.h"

class Particle
{
public:

	Particle();
	virtual ~Particle();

	static void init();

private:
	static Wog* spWogInstance;

};

