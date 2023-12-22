#include "Zombie.h"

#include "Human.h"

#include <GameEngine/ResourceManager.h>
#include <GameEngine/GameEngineErrors.h>
#include <GameEngine/AudioEngine.h>

Zombie::Zombie()
{

}

Zombie::~Zombie()
{

}

void Zombie::init(float speed, glm::vec2 pos)
{
	_speed = speed;
	_position = pos;
	m_zombieDeath = m_audioEngine.loadSoundEffect("Sound/shots/zombieDeath.wav");


	GameEngine::checkNan(_position);

	_health = 50;

	_color = GameEngine::ColorRGBA8(255, 255, 255, 255);

	m_textureID = GameEngine::ResourceManager::getTexture("Textures/zombie.png").id;
}

void Zombie::update(const std::vector<std::string>& levelData, std::vector<Human*>& humans, std::vector<Zombie*>& zombies, float deltaTime)
{

	Human* closestHuman = getNearestHuman(humans);

	if (closestHuman != nullptr) {
		_direction = glm::normalize(closestHuman->getPosition() - _position);
		_position += _direction * _speed * deltaTime;
		GameEngine::checkNan(_position);
	}

	// Do collision
	collideWithLevel(levelData);
}


Human* Zombie::getNearestHuman(std::vector<Human*>& humans)
{
	Human* closestHuman = nullptr;
	float smallestDistance = 99999999.0f;

	for (int i = 0; i < humans.size(); i++) {
		glm::vec2 distVec = humans[i]->getPosition() - _position;
		float distance = glm::length(distVec);

		if (distance < smallestDistance) {
			smallestDistance = distance;
			closestHuman = humans[i];
		}
	}

	return closestHuman;
}

