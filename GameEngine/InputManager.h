#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

namespace GameEngine {
	
	class InputManager
	{
	public:
		InputManager();
		~InputManager();
		
		void update();

		void keyPress(unsigned int keyID);
		void releaseKey(unsigned int keyID);

		void setMouseCoords(float x, float y);

		//returns true if key is held down
		bool isKeyDown(unsigned int keyID);

		//returns true if key is just pressed 
		bool isKeyPressed(unsigned int keyID);

		//getters
		glm::vec2 getMouseCoord() const { return _mouseCoords; }

	private:
		//returns true if key was held down
		bool wasKeyDown(unsigned int keyID);

		std::unordered_map<unsigned int, bool> _keyMap;
		std::unordered_map<unsigned int, bool> _previousKeyMap;

		glm::vec2 _mouseCoords;

	};

}