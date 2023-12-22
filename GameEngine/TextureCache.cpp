#include "TextureCache.h"
#include "imageLoader.h"

#include <iostream>


namespace GameEngine {
	TextureCache::TextureCache()
	{

	}
	TextureCache::~TextureCache()
	{

	}

	GLTexture TextureCache::getTexture(std::string texturePath)
	{
		//lookup the texture and see if its in the map
		auto mit = _textureMap.find(texturePath);

		//check if its not in the map
		if (mit == _textureMap.end()) {
			//load the texture
			GLTexture newTexture = imageLoader::loadPNG(texturePath);

			//insert it into the map
			_textureMap.insert(make_pair(texturePath, newTexture));


			//std::cout << "Loaded texture!\n";
			return newTexture;
		}

		//std::cout << "Used Cached texture!\n";
		return mit->second;

	}
}