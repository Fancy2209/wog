#pragma once

#include "Wog.h"

class GooBall
{
public:

	GooBall();
	virtual ~GooBall();

	static void init();

private:
	static Wog* spWogInstance;

};

