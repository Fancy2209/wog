#pragma once

#include <map>
#include <string>
#include "Boy/KeyboardListener.h"
#include "Boy/MouseListener.h"
//#include "InputEventHandler.h"

class GooController: public Boy::KeyboardListener, Boy::MouseListener//, InputEventHandler 
{
public:

	GooController();
	virtual ~GooController();

	virtual void keyUp(wchar_t unicode, Boy::Keyboard::Key key, Boy::Keyboard::Modifiers mods);
	virtual void keyDown(wchar_t unicode, Boy::Keyboard::Key key, Boy::Keyboard::Modifiers mods);

	virtual void mouseMove(Boy::Mouse *mouse) = 0;
	virtual void mouseButtonDown(Boy::Mouse *mouse, Boy::Mouse::Button button, int clickCount) = 0;
	virtual void mouseButtonUp(Boy::Mouse *mouse, Boy::Mouse::Button button) = 0;
	virtual void mouseWheel(Boy::Mouse *mouse, int wheelDelta) = 0;
	virtual void mouseEnter(Boy::Mouse *mouse) = 0;
	virtual void mouseLeave(Boy::Mouse *mouse) = 0;
};

