#include "imageLoader.h"
#include "picopng.h"
#include "IOManager.h"
#include "GameEngineErrors.h"



namespace GameEngine {
	GLTexture imageLoader::loadPNG(std::string filePath)
	{
		//create a GLTexture and initialize all its fields to 0
		GLTexture texture = {};

		//this is the input data to decodePNGm which we load from a file
		std::vector<unsigned char> in;
		//this is the output data from decodePNG, which is the pixel data for our texture
		std::vector<unsigned char> out;

		unsigned long width, heigth;

		//read in the image file contents into a buffer
		if (IOManager::readFileToBuffer(filePath, in) == false) {
			fatalError("Failed to load PNG file to buffer!");
		}

		//decode the .png format into an array of pixels
		int errorCode = decodePNG(out, width, heigth, &(in[0]), in.size());
		if (errorCode != 0) {
			fatalError("decodePNG failed with error: " + std::to_string(errorCode));
		}

		//generate the openGL texture object
		glGenTextures(1, &(texture.id));

		//bind the texture object
		glBindTexture(GL_TEXTURE_2D, texture.id);
		//upload the pixels to the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(out[0]));

		//set some texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//generate the mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		//unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);

		texture.width = width;
		texture.heigth = heigth;

		//return a copy of the texture data
		return texture;
	}
}