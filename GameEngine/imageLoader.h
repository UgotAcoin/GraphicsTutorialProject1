#pragma once
#include "GLTexture.h"
#include <string>

namespace GameEngine {
	//loads images into GLTextures
	class imageLoader
	{
	public:
		static GLTexture loadPNG(std::string filePath);

	};

}