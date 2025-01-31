#pragma once

#include <map>
#include <string>

class BallFactory
{
public:

	BallFactory();
	virtual ~BallFactory();

	static void init();
	static BallFactory* instance();

private:
	static BallFactory* gInstance;
	static std::map<std::string, int/*GooBall::SoundEvent*/> gSoundMap;
};

