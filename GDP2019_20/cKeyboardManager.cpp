#include "cKeyboardManager.hpp"

int cKeyboardManager::current_keys[GLFW_KEY_LAST] = { 0, };
int cKeyboardManager::previous_keys[GLFW_KEY_LAST] = { 0, };
int cKeyboardManager::current_mods = 0;
int cKeyboardManager::previous_mods = 0;

void cKeyboardManager::handleKeyboardInput(int key, int scancode, int action, int mods)
{
	current_keys[key] = action;

	previous_mods = current_mods;
	current_mods = mods;
}

void cKeyboardManager::update()
{
	memcpy(previous_keys, current_keys, GLFW_KEY_LAST * sizeof(int));
}

int cKeyboardManager::keyStatus(int key)
{
	return current_keys[key];
}

bool cKeyboardManager::keyUp(int key)
{
	return !current_keys[key];
}

bool cKeyboardManager::keyDown(int key)
{
	return current_keys[key];
}

bool cKeyboardManager::keyRepeating(int key)
{
	return current_keys[key] == GLFW_REPEAT;
}

bool cKeyboardManager::keyPressed(int key)
{
	bool ret = (!previous_keys[key] && current_keys[key]);
	//if (ret)
	//	previous_keys[key] = current_keys[key];
	return ret;
}

bool cKeyboardManager::keyReleased(int key)
{
	bool ret = (previous_keys[key] && !current_keys[key]);
	//if (ret)
	//	previous_keys[key] = current_keys[key];
	return ret;
}
