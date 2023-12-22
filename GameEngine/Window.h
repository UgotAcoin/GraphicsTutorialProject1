#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <string>

enum windowflags {INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4};

namespace GameEngine {
	class Window
	{
	public:
		Window();
		~Window();

		int create(std::string windowName, int screenWidth, int screenHeigth, unsigned int currentFlags);

		void swapBuffer();

		int getScreenWidth() { return _screenWidth; }
		int getScreenHeigth() { return _screenHeigth; }

	private:
		SDL_Window* _sdlWindow;
		int _screenWidth, _screenHeigth;

	};

}