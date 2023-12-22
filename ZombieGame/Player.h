#pragma once

#include "Human.h"
#include "Bullet.h"

#include <GameEngine/InputManager.h>
#include <GameEngine/Camera2D.h>

class Gun;

class Player : public Human
{
public:
	Player();
	~Player();
	
	void init(float speed, glm::vec2 pos, GameEngine::InputManager* inputManager, GameEngine::Camera2D* camera, std::vector<Bullet>* bullets);

	void addGun(Gun* gun);

	void update(const std::vector<std::string>& levelData, std::vector<Human*>& humans, std::vector<Zombie*>& zombies, float deltaTime) override;

private:
	GameEngine::InputManager* m_inputManager;

	std::vector<Gun*> _guns;
	int _currentGunIndex;

	GameEngine::Camera2D* m_camera;
	std::vector<Bullet>* m_bullets;

};

