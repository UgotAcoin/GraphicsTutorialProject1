#include "Player.h"

#include "Gun.h"

#include <SDL/SDL.h>

#include <GameEngine/ResourceManager.h>
#include <GameEngine/GameEngineErrors.h>

Player::Player() :
	_currentGunIndex(-1)
{

}

Player::~Player()
{

}


void Player::init(float speed, glm::vec2 pos, GameEngine::InputManager* inputManager, GameEngine::Camera2D* camera, std::vector<Bullet>* bullets)
{
	_speed = speed;
	_position = pos;

	GameEngine::checkNan(_position);

	m_inputManager = inputManager;
	m_camera = camera;
	m_bullets = bullets;

	_health = 150;

	_color = GameEngine::ColorRGBA8(255, 255, 255, 255);

	m_textureID = GameEngine::ResourceManager::getTexture("Textures/player.png").id;
}

void Player::addGun(Gun* gun)
{
	// add a gun to inventory
	_guns.push_back(gun);

	//if no gun equipped, equip gun
	if (_currentGunIndex == -1) {
		_currentGunIndex = 0;
	}
}

void Player::update(const std::vector<std::string>& levelData,
					 	  std::vector<Human*>& humans, 
						  std::vector<Zombie*>& zombies, 
						  float deltaTime)
{
	if (m_inputManager->isKeyDown(SDLK_w)) {
		_position.y += _speed * deltaTime;
		GameEngine::checkNan(_position);

	} else if (m_inputManager->isKeyDown(SDLK_s)) {
		_position.y -= _speed * deltaTime;
		GameEngine::checkNan(_position);
	}
	if (m_inputManager->isKeyDown(SDLK_a)) {
		_position.x -= _speed * deltaTime;
		GameEngine::checkNan(_position);
	} else if (m_inputManager->isKeyDown(SDLK_d)) {
		_position.x += _speed * deltaTime;
		GameEngine::checkNan(_position);
	}
	if (m_inputManager->isKeyDown(SDLK_1) && _guns.size() >= 0) {
		_currentGunIndex = 0;
	} else if (m_inputManager->isKeyDown(SDLK_2) && _guns.size() >= 1) {
		_currentGunIndex = 1;
	} else if (m_inputManager->isKeyDown(SDLK_3) && _guns.size() >= 2) {
		_currentGunIndex = 2;
	}

	glm::vec2 mouseCoords = m_inputManager->getMouseCoord();
	mouseCoords = m_camera->convertScreenToWorld(mouseCoords);

	glm::vec2 centerPos = _position + glm::vec2(AGENT_RADIUS);

	_direction = glm::normalize(mouseCoords - centerPos);

	if (_currentGunIndex != -1) {

		_guns[_currentGunIndex]->update(m_inputManager->isKeyDown(SDL_BUTTON_LEFT), centerPos, _direction, *m_bullets, deltaTime);
	}

	collideWithLevel(levelData);
}

