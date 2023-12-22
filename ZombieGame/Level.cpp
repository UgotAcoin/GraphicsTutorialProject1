#include "Level.h"

#include <GameEngine/GameEngineErrors.h>
#include <GameEngine/ResourceManager.h>

#include <fstream>
#include <iostream>

Level::Level(const std::string fileName)
{
	std::ifstream file;
	file.open(fileName);

	//error checking
	if (file.fail()) {
		GameEngine::fatalError("Failed to open: " + fileName);
	}

	//throw away the first string in tmp
	std::string tmp;
	file >> tmp >> m_numHumans;
	std::getline(file, tmp); //throw away the rest of the first line

	//reads the level data
	while (std::getline(file, tmp)) {
		m_levelData.emplace_back(tmp);
	}

	_spriteBatch.init();
	_spriteBatch.begin();

	glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	GameEngine::ColorRGBA8 whiteColor;
	whiteColor.r = 255;
	whiteColor.g = 255;
	whiteColor.b = 255;
	whiteColor.a = 255;

	for (int y = 0; y < m_levelData.size(); y++) {
		for (int x = 0; x < m_levelData[y].size(); x++) {
			//grab tile
			char tile = m_levelData[y][x];

			//get dest rect
			glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);

			//process the tile
			switch (tile) {
			case 'R':
				_spriteBatch.draw(destRect,
									uvRect, 
									GameEngine::ResourceManager::getTexture("Textures/red_bricks.png").id,
									0.0f,
									whiteColor);
				break;
			case 'L':
				_spriteBatch.draw(destRect,
									uvRect,
									GameEngine::ResourceManager::getTexture("Textures/light_bricks.png").id,
									0.0f,
									whiteColor);
				break;
			case 'G':
				_spriteBatch.draw(destRect,
									uvRect,
									GameEngine::ResourceManager::getTexture("Textures/glass.png").id,
									0.0f,
									whiteColor);
				break;
			case '@':
				m_levelData[y][x] = '.'; //so we dont collide with a @
				_startPlayerPos.x = x * TILE_WIDTH;
				_startPlayerPos.y = y * TILE_WIDTH;
				break;
			case 'Z':
				m_levelData[y][x] = '.'; //so we dont collide with a Z
				_zombieStartPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
				break;
			case '.':
				break;
			default:
				std::printf("Unexpected symbol %c at (%d, %d)", tile, x, y);
				break;
			}
		}
	}

	_spriteBatch.end();


}

Level::~Level()
{

}

void Level::draw()
{
	_spriteBatch.renderBatch();
}

