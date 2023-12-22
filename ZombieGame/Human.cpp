#include "Human.h"

#include <random>
#include <ctime>

#include <GameEngine/ResourceManager.h>
#include <GameEngine/GameEngineErrors.h>
#include <glm/gtx/rotate_vector.hpp>


Human::Human() : _frames(0)
{

}

Human::~Human()
{

}

void Human::init(float speed, glm::vec2 pos)
{
    static std::mt19937 randomEngine(time(nullptr));
    static std::uniform_real_distribution<float> randDir(-1.0f, 1.0f);

    _health = 20;

    _color = GameEngine::ColorRGBA8(255, 255, 255, 255);

    _speed = speed;
    _position = pos;
    GameEngine::checkNan(_position);

    // Get random direction
    _direction = glm::vec2(randDir(randomEngine), randDir(randomEngine));
    // Make sure direction isn't zero
    if (_direction.length() == 0) _direction = glm::vec2(1.0f, 0.1f);

    _direction = glm::normalize(_direction);

	m_textureID = GameEngine::ResourceManager::getTexture("Textures/human.png").id;

}

void Human::update(const std::vector<std::string>& levelData,
                    std::vector<Human*>& humans,
                    std::vector<Zombie*>& zombies,
                    float deltaTime)
{ 
    static std::mt19937 randomEngine(time(nullptr));
    static std::uniform_real_distribution<float> randRot(-0.25f, 0.25f);

	_position += _direction * _speed * deltaTime;
    GameEngine::checkNan(_position);


    if (_frames == 20) {
        _direction = glm::rotate(_direction, randRot(randomEngine));
        _frames = 0;
    } else {
        _frames++;
    }

    if (collideWithLevel(levelData)) {
        _direction = glm::rotate(_direction, randRot(randomEngine));
    }
}