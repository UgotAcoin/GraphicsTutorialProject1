#include "InputManager.h"

namespace GameEngine {

	InputManager::InputManager() : _mouseCoords(0.0f)
	{

	}
	InputManager::~InputManager()
	{

	}

	void InputManager::update()
	{
		//loop through _keyMap using a for each loop, and copy it over to _perviousKeyMap
		for (auto& it : _keyMap) {
			_previousKeyMap[it.first] = it.second;
		}
	}

	void InputManager::keyPress(unsigned int keyID)
	{
		_keyMap[keyID] = true;
	}

	void InputManager::releaseKey(unsigned int keyID)
	{
		_keyMap[keyID] = false;

	}

	void InputManager::setMouseCoords(float x, float y)
	{
		_mouseCoords.x = x;
		_mouseCoords.y = y;
	}

	bool InputManager::isKeyDown(unsigned int keyID)
	{
		//we dont want to use the associative array approach here
		// because we dont want to creat a key if it doesnt exist.
		//so we do it manually
		auto it = _keyMap.find(keyID);
		if (it != _keyMap.end()) {
			// found the key
			return it->second;
		} else {
			//didnt find the key
			return false;
		}
	}

	bool InputManager::isKeyPressed(unsigned int keyID)
	{
		//check if it is pressed this frame, and wasnt pressed last frame 
		if (isKeyDown(keyID) == true && wasKeyDown(keyID) == false) {
			return true;
		}
		return false;
	}

	bool InputManager::wasKeyDown(unsigned int keyID)
	{
		//we dont want to use the associative array approach here
		// because we dont want to creat a key if it doesnt exist.
		//so we do it manually
		auto it = _previousKeyMap.find(keyID);
		if (it != _previousKeyMap.end()) {
			// found the key
			return it->second;
		}
		else {
			//didnt find the key
			return false;
		}
	}

}
