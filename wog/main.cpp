#include "Boy/Environment.h"
#include "Wog.h"
#include <time.h>

int main(int argc, char* argv[])
{
	unsigned char KEY[] = { 0x0D, 0x06, 0x07, 0x07, 0x0C, 0x01, 0x08, 0x05,
		  0x06, 0x09, 0x09, 0x04, 0x06, 0x0D, 0x03, 0x0F,
		  0x03, 0x06, 0x0E, 0x01, 0x0E, 0x02, 0x07, 0x0B };

	// initialize the platform:
	clock_t startTime = clock();
	Boy::Environment::instance()->init(
		Wog::instance(),
		800, // width
		600, // height
		false, // fullscreen
		"World of Goo", // window title
		"pers2.dat", // persistence layer file name
		KEY // AES Key for assets, set to null to not use encryption like WoG 1.5
	);

	// start the main loop
	Boy::Environment::instance()->startMainLoop();

	clock_t endTime = clock();
	Boy::Environment::instance()->debugLog("TotalRunningTime = %d seconds\n", (endTime - startTime) / 1000);

	// destroy the game:
	Wog::destroy();

	// destroy the environment:
	Boy::Environment::instance()->destroy();

	return 0;
}

