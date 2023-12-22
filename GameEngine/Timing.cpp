#include "Timing.h"

namespace GameEngine {

	FpsLimiter::FpsLimiter()
	{

	}

	void FpsLimiter::init(float maxFPS)
	{
		setMaxFPS(maxFPS);
	}

	void FpsLimiter::setMaxFPS(float maxFPS)
	{
		_maxFPS = maxFPS;
	}

	void FpsLimiter::begin()
	{
		_startTicks = SDL_GetTicks();
	}

	// end will return the cureent fps
	float FpsLimiter::end()
	{
		calculateFPS();

		float frameTicks = SDL_GetTicks() - _startTicks;
		//limite the max fps
		if (1000.0f / _maxFPS > frameTicks) {
			SDL_Delay(1000.0f / _maxFPS - frameTicks);
		}

		return _fps;

	}

	void FpsLimiter::calculateFPS()
	{
		//the number of frames to average
		static const int NUM_SAMPLES = 10;
		//stores all the frametimes for each frame that we will average
		static float frameTimes[NUM_SAMPLES];
		//the current frame we are on
		static int currentFrame = 0;
		//the ticks of the previous frame
		static float prevTicks = SDL_GetTicks();

		//ticks for the current frame
		float currentTicks = SDL_GetTicks();

		//calculate the number of ticks (ms) for this frame
		_frameTime = currentTicks - prevTicks;
		frameTimes[currentFrame % NUM_SAMPLES] = _frameTime;

		//current ticks is now previous ticks
		prevTicks = currentTicks;

		//the number of frames to average
		int count;

		currentFrame++;
		if (currentFrame < NUM_SAMPLES) {
			count = currentFrame;
		}
		else {
			count = NUM_SAMPLES;
		}

		//average all the frame times
		float frameTimeAverage = 0;
		for (int i = 0; i < count; i++) {
			frameTimeAverage += frameTimes[i];
		}
		frameTimeAverage /= count;

		//calculate fps
		if (frameTimeAverage > 0) {
			_fps = 1000.0 / frameTimeAverage;
		}
		else {
			_fps = 60.0f;
		}

	}

}