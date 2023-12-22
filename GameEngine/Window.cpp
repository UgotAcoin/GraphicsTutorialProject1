#include "Window.h"
#include "GameEngineErrors.h"


namespace GameEngine {
	Window::Window()
	{

	}
	Window::~Window()
	{

	}

	int Window::create(std::string windowName, int screenWidth, int screenHeigth, unsigned int currentFlags)
	{
		Uint32 flags = SDL_WINDOW_OPENGL;

		if (currentFlags & INVISIBLE) {
			flags |= SDL_WINDOW_HIDDEN;
		}

		if (currentFlags & FULLSCREEN) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		if (currentFlags & BORDERLESS) {
			flags |= SDL_WINDOW_BORDERLESS;
		}

		//open an SDL window
		_sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeigth, flags);
		if (_sdlWindow == nullptr) {
			fatalError("SDL Window could not be created!");
		}
		//set up our OpenGL conext
		SDL_GLContext glContext = SDL_GL_CreateContext(_sdlWindow);
		if (glContext == nullptr) {
			fatalError("SDL Window could not be created!");
		}
		//set up glew
		GLenum error = glewInit();
		if (error != GLEW_OK) {
			fatalError("Could not initialize glew!");
		}

		//check the OpenGL version
		std::printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));
		//set the color
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		//set vsync
		SDL_GL_SetSwapInterval(0);

		//enable alpha blend
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		return 0;
	}

	void Window::swapBuffer()
	{
		SDL_GL_SwapWindow(_sdlWindow);
	}
}