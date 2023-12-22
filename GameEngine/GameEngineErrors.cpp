#include "GameEngineErrors.h"

#include <cstdlib>

#include <iostream>
#include <SDL/SDL.h>

namespace GameEngine {
	//output for errors
	void fatalError(std::string errorString) {
		std::cout << errorString << std::endl;
		std::cout << "Enter any key to quit...\n";
		int tmp;
		std::cin >> tmp;
		SDL_QUIT;
		exit(69);
	}

	void checkNan(glm::vec2& testA) {
		if (std::isnan(testA.x)) {
			throw std::invalid_argument("X is nan!\n");
		}


		if (std::isnan(testA.y)) {
			throw std::invalid_argument("Y is nan!\n");
		}
	}

}