#include <SDL/SDL.h>
#include <GL/glew.h>

#include "GameEngine.h"

namespace GameEngine {
	int init() {
		//initalize SDL
		SDL_Init(SDL_INIT_EVERYTHING);

		//tel SDL that we want a double buffered window so we dont get 
		//any flickering
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		return 0;
	}
}