#include "Bullet.h"

#include <GameEngine/GLTexture.h>
#include <GameEngine/ResourceManager.h>

Bullet::Bullet(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime)
{
	_position = pos;
	_direction = dir;
	_speed = speed;
	_lifeTime = lifeTime;
}

Bullet::~Bullet()
{

}


void Bullet::draw(GameEngine::SpriteBatch& spriteBatch)
{
	glm::vec4 posAndSize = glm::vec4(_position.x, _position.y, 20, 20);
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static GameEngine::GLTexture texture = GameEngine::ResourceManager::getTexture("Textures/jimmyJump_pack/PNG/Bullet.png");
	GameEngine::ColorRGBA8 color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;


	spriteBatch.draw(posAndSize, uv, texture.id, 0.0f, color);
}

bool Bullet::update()
{
	_position += _direction * _speed;
	_lifeTime--;
	if (_lifeTime == 0) {
		return true;
	}
	return false;
}