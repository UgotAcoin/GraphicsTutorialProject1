#pragma once

#include <GameEngine/Sprite.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/Window.h>
#include <GameEngine/GameEngine.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/InputManager.h>
#include <GameEngine/Timing.h>

#include "Bullet.h"

#include <SDL\SDL.h>
#include <GL\glew.h>

#include <vector>


enum class GameState { PLAY, EXIT };

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();



private:

	void initSystems();
	void initShaders();
	void gameLoop();
	void processInput();
	void drawGame();

	GameEngine::Window _window;
	int _screenWidth;
	int _screenHeigth;
	GameState _gameState;


	GameEngine::GLSLProgram _colorProgram;
	GameEngine::Camera2D _camera;
	GameEngine::SpriteBatch _spriteBatch;
	GameEngine::InputManager _inputManager;
	GameEngine::Fpslimiter _fpsLimiter;

	std::vector<Bullet> _bullets;

	float _maxFPS;
	float _fps;

	float _time;

};

