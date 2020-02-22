#pragma once

#include "GLCommon.h"


class cKeyboardManager
{
	static int current_keys[GLFW_KEY_LAST];
	static int previous_keys[GLFW_KEY_LAST];
	static int current_mods;
	static int previous_mods;

public:
	static void handleKeyboardInput(int key, int scancode, int action, int mods);
	static void update();

	static int keyStatus(int key);

	static bool keyUp(int key);
	static bool keyDown(int key);
	static bool keyRepeating(int key);

	static bool keyPressed(int key);
	static bool keyReleased(int key);
};
