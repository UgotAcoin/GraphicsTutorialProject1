#pragma once

#include <string>

#include <glm/glm.hpp>

namespace GameEngine {
	extern void fatalError(std::string errorString);

	extern void checkNan(glm::vec2& testA);

}