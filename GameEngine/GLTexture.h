#pragma once

#include <GL/glew.h>
#include <string>

namespace GameEngine {
	struct GLTexture {
		GLuint id;
		int width;
		int heigth;
	};
}