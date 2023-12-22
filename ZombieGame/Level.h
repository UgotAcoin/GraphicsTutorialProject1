#pragma once

#include <string>
#include <vector>

#include <GameEngine/SpriteBatch.h>

const float TILE_WIDTH = 64.0f;

class Level
{
public:
	//load the level
	Level(const std::string fileName);
	~Level();

	void draw();

	//getters
	size_t getWidth() const { return m_levelData[0].size(); }
	size_t getHeigth() const { return m_levelData.size(); }
	int getNumHumans() const { return m_numHumans; }
	const std::vector<std::string>& getLevelData() const { return m_levelData; }
	glm::vec2 getStartPlayerPos() const { return _startPlayerPos; }
	const std::vector<glm::vec2>& getZombieStartPositions() const { return _zombieStartPositions; }

private:
	std::vector<std::string> m_levelData;
	int m_numHumans;
	GameEngine::SpriteBatch _spriteBatch;

	glm::vec2 _startPlayerPos;
	std::vector<glm::vec2> _zombieStartPositions;

};

