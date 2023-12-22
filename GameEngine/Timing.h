#pragma once

#include <SDL/SDL.h>

namespace GameEngine {

	class FpsLimiter {

	public:
		FpsLimiter();

		void init(float _maxFPS);

		void setMaxFPS(float _maxFPS);

		void begin();

		// end will return the cureent fps
		float end();

	private:
		void calculateFPS();

		float _fps;
		float _maxFPS;
		float _frameTime;
		unsigned int _startTicks;

	};

}