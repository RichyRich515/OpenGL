#pragma once

#include "GLCommon.h"


class cKeyboardManager
{
	int current_keys[GLFW_KEY_LAST];
	int previous_keys[GLFW_KEY_LAST];
	int current_mods;
	int previous_mods;

public:
	void handleKeyboardInput(int key, int scancode, int action, int mods);

	int keyStatus(int key);

	bool keyUp(int key);
	bool keyDown(int key);
	bool keyRepeating(int key);
		 
	bool keyPressed(int key);
	bool keyReleased(int key);
};
