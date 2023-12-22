#pragma once

#include <GameEngine/Window.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/InputManager.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/SpriteFont.h>
#include <GameEngine/AudioEngine.h>
#include <GameEngine/ParticleEngine2D.h>
#include <GameEngine/ParticleBatch2D.h>

#include <random>
#include <ctime>

#include "Player.h"
#include "Level.h"
#include "Bullet.h"

class Zombie;

enum class GameState {
	PLAY, 
	EXIT
};

class MainGame
{
public:
	MainGame();
	~MainGame();

	//runs the game
	void run();

private:
	//initializes the core system
	void initSystems();
	
	//initializes the level and sets it up
	void initLevel();

	//initializes the shaders
	void initShaders();

	//main game loop for the program
	void gameLoop();

	//handles input processing
	void processInput();

	//renders the game
	void drawGame();

	//renders the hud
	void drawHud();

	//Draws blood
	void addBlood(const glm::vec2& position, int numParticles);

	//updates all agents
	void updateAgents(float deltaTime); 

	//updates all bullets
	void updateBullets(float deltaTime);

	//checks the victory condition
	void checkVictory();

	//checks for 3 thing (if they are in the same spawn pos/ 
	//					  if they spawn outside the world border/ 
	//					  if they spawn out on an air space)
	bool checkSpawnPos(const std::vector<std::string>& levelData, glm::vec2 pos);

	//makes a random number
	glm::vec2 randPos();

	//member variables
	GameEngine::Window m_window; //<- the game window

	GameEngine::GLSLProgram m_textureProgram; //<- the shader program

	GameEngine::InputManager m_inputManager; //<- handles input

	GameEngine::Camera2D m_camera; //<- main camera
	GameEngine::Camera2D m_hudCamera; ///< Hud Camera

	GameEngine::SpriteBatch m_agentSpriteBatch; //<- draws all agents
	GameEngine::SpriteBatch m_hudSpriteBatch;

	GameEngine::ParticleEngine2D m_particleEngine;
	GameEngine::ParticleBatch2D* m_bloodParticleBatch;

	std::vector<Level*> m_levels; //<- vector of all levels

	int m_screenWidth = 1024;
	int	m_screenHeight = 768;

	float m_fps;

	int m_currentLevel;

	Player* m_player;

	std::vector<Human*> m_humans; //<- Vector of all humans
	std::vector<Zombie*> m_zombies; //<- Vector of all zombies
	std::vector<Bullet> m_bullets; //<- Vector of all bullets

	int m_numHumansKilled; //<- number of humans player killed
	int m_numZombiesKilled; //<- number of zombies player killed

	GameEngine::SpriteFont* m_spriteFont;

	GameEngine::AudioEngine m_audioEngine;

	GameState m_gameState;

	std::mt19937 m_randomEngine;
	


};

