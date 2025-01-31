#pragma once


class SceneFactory
{
public:

	SceneFactory();
	virtual ~SceneFactory();

	static void init();
	static SceneFactory* instance();

private:
	static SceneFactory* gInstance;

};

