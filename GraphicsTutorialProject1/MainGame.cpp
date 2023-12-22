#include "MainGame.h"

#include <GameEngine/Error.h>
#include <GameEngine/ResourceManager.h>

#include <iostream>
#include <string>


MainGame::MainGame() : 
	_screenWidth(1024),
	_screenHeigth(768),
	_gameState(GameState::PLAY),
	_time(0.0f),
	_maxFPS(60.0f)
{
	_camera.init(_screenWidth, _screenHeigth);
}
MainGame::~MainGame()
{

}

//what runs the game 
void MainGame::run()
{
	initSystems();

	//this only returns when game ends
	gameLoop();
}

//initilizing the window screen("Display")
void MainGame::initSystems()
{
	GameEngine::init();

	_window.create("Game Engine", _screenWidth, _screenHeigth, 0);

	initShaders();

	_spriteBatch.init();
	_fpsLimiter.init(_maxFPS);

	

}

//initilizing the shaders
void MainGame::initShaders()
{
	_colorProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.addAttribute("vertexColor");
	_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();
}

//loop to keep the game running
void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT) {
		
		_fpsLimiter.begin();

		processInput();
		_time += 0.01;

		_camera.update();

		//update all bullets
		for (int i = 0; i < _bullets.size();) {
			if (_bullets[i].update() == true) {
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
			} else {
				i++;
			}
		}

		drawGame();

		_fps = _fpsLimiter.end();

		//print only once every 10 frames
		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10000) {
			std::cout << _fps << std::endl;
			frameCounter = 0;
		}

	}
}

//tracks the input to the window
void MainGame::processInput()
{
	const float CAMERA_SPEED = 1.5f;
	const float SCALE_SPEED = 0.01f;

	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			//std::cout << evnt.motion.x << " " << evnt.motion.y << std::endl;
			_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			_inputManager.keyPress(evnt.key.keysym.sym);
			break;
		case SDL_KEYUP:
			_inputManager.releaseKey(evnt.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_inputManager.keyPress(evnt.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			_inputManager.releaseKey(evnt.button.button);
			break;
		}
	}

	if (_inputManager.isKeyDown(SDLK_w)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0, -CAMERA_SPEED));
	}
	if (_inputManager.isKeyDown(SDLK_s)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0, CAMERA_SPEED));
	}
	if (_inputManager.isKeyDown(SDLK_a)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(CAMERA_SPEED, 0.0f));
	}
	if (_inputManager.isKeyDown(SDLK_d)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(-CAMERA_SPEED, 0.0f));
	}
	if (_inputManager.isKeyDown(SDLK_q)) {
		_camera.setScale(_camera.getScale() + SCALE_SPEED);
	}
	if (_inputManager.isKeyDown(SDLK_e)) {
		_camera.setScale(_camera.getScale() - SCALE_SPEED);
	}
	if (_inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
		glm::vec2 mouseCoords = _inputManager.getMouseCoord();
		mouseCoords = _camera.convertScreenToWorld(mouseCoords);
	
		glm::vec2 playerPosition(0.0f);
		glm::vec2 direction = mouseCoords - playerPosition;
		direction = glm::normalize(direction);

		_bullets.emplace_back(playerPosition, direction, 1.0f, 1000);

	}
}

//draws objects to the screen
void MainGame::drawGame()
{

	//set the base depth to 1.0
	glClearDepth(1.0);
	//clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//enable the shader
	_colorProgram.use();

	//we are using texture unit 0
	glActiveTexture(GL_TEXTURE0);
	//get the uniform location
	GLint textureLocation = _colorProgram.getUniformLocation("mySampler");
	//tell the shader that the texture is in texture unit 0
	glUniform1i(textureLocation, 0);

	//set the constanly changing time variable
//	GLuint timeLocation = _colorProgram.getUniformLocation("time");
//	glUniform1f(timeLocation, _time);

	//set the camera matrix
	GLuint plocation = _colorProgram.getUniformLocation("P");
	glm::mat4 cameraMatrix = _camera.getCameraMatrix();

	glUniformMatrix4fv(plocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	_spriteBatch.begin();

	glm::vec4 pos(0.0f, 0.0f, 50.0f, 50.0f);
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);

	static GameEngine::GLTexture texture = GameEngine::ResourceManager::getTexture("Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
	GameEngine::ColorRGBA8 color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	_spriteBatch.draw(pos, uv, texture.id, 0.0f, color);
	//_spriteBatch.draw(pos + glm::vec4(50, 0, 0, 0), uv, texture.id, 0.0f, color);

	for (int i = 0; i < _bullets.size(); i++) {
		_bullets[i].draw(_spriteBatch);
	}

	

	_spriteBatch.end();

	_spriteBatch.renderBatch();

	//unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//disables the shader
	_colorProgram.unuse();

	//swap our buffer and draw everything to the screen
	_window.swapBuffer();
}

