#include "Agent.h"

#include <GameEngine/ResourceManager.h>
#include <GameEngine/GameEngineErrors.h>


#include <iostream>
#include <cstdlib>
#include <string>


#include "Level.h"
#include <algorithm>

Agent::Agent()
{
}


Agent::~Agent()
{
}

bool Agent::collideWithLevel(const std::vector<std::string>& levelData) {

    std::vector<glm::vec2> collideTilePositions;

    // Check the four corners
    // First corner
    checkTilePosition(levelData,
        collideTilePositions,
        _position.x,
        _position.y);
    

    // Second Corner
    checkTilePosition(levelData,
        collideTilePositions,
        _position.x + AGENT_WIDTH,
        _position.y);

    // Third Corner
    checkTilePosition(levelData,
        collideTilePositions,
        _position.x,
        _position.y + AGENT_WIDTH);

    // Forth Corner
    checkTilePosition(levelData,
        collideTilePositions,
        _position.x + AGENT_WIDTH,
        _position.y + AGENT_WIDTH);

    // Check if there was no collision
    if (collideTilePositions.size() == 0) {
        return false;
    }

    // Do the collision
    for (int i = 0; i < collideTilePositions.size(); i++) {
        collideWithTile(collideTilePositions[i]);
    }
    return true;
}

// Circular collision
bool Agent::collideWithAgent(Agent* agent) {

    if (this == agent) {
        throw std::invalid_argument("Checking with colliction with self");
    }

    // Minimum distance before there is a collision
    const float MIN_DISTANCE = AGENT_RADIUS * 2.0f;

    // Center position of this agent
    glm::vec2 centerPosA = _position + glm::vec2(AGENT_RADIUS);

    // Center position of the parameter agent
    glm::vec2 centerPosB = agent->getPosition() + glm::vec2(AGENT_RADIUS);

    if (centerPosA == centerPosB) {
        throw std::invalid_argument("The center poses are the same");
    }


    // Distance vector between the two agents
    glm::vec2 distVec = centerPosA - centerPosB;

    // Length of the distance vector
    float distance = glm::length(distVec);

    // Depth of the collision
    float collisionDepth = MIN_DISTANCE - distance;

    // If collision depth > 0 then we did collide
    if (collisionDepth > 0) {

        // Get the direction times the collision depth so we can push them away from each other
        glm::vec2 collisionDepthVec = glm::normalize(distVec) * collisionDepth;

        // Push them in opposite directions
        _position += collisionDepthVec / 2.0f;

        GameEngine::checkNan(_position);

        agent->_position -= collisionDepthVec / 2.0f;

        GameEngine::checkNan(agent->_position);

        return true;
    }
    return false;
}

void Agent::draw(GameEngine::SpriteBatch& _spriteBatch) {

    const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

    glm::vec4 destRect;
    destRect.x = _position.x;
    destRect.y = _position.y;
    destRect.z = AGENT_WIDTH;
    destRect.w = AGENT_WIDTH;

    _spriteBatch.draw(destRect, uvRect, m_textureID, 0.0f, _color, _direction);
}

bool Agent::applyDamage(float damage) {

    _health -= damage;
    // If we died, return true
    if (_health <= 0) {
        return true;
    }
    return false;
}

void Agent::checkTilePosition(const std::vector<std::string>& levelData,
    std::vector<glm::vec2>& collideTilePositions,
    float x,
    float y) {

    // Get the position of this corner in grid-space
    glm::vec2 gridPos = glm::vec2(floor(x / TILE_WIDTH), floor(y / TILE_WIDTH));

    // If we are outside the world, just return
    if (gridPos.x < 0 || gridPos.x >= levelData[0].size() ||
        gridPos.y < 0 || gridPos.y >= levelData.size()) {
        return;
    }


    std::string row = levelData[gridPos.y];
    char cell = row[gridPos.x];
    
    if (cell != '.') {

        glm::vec2 A = gridPos * TILE_WIDTH + glm::vec2(TILE_WIDTH / 2.0f);
        collideTilePositions.push_back(A);
    }
}

// AABB (Axis Aligned Bounding Box) collision
void Agent::collideWithTile(glm::vec2 tilePos) {

   // printf("Collide with tile begin \n");

    const float TILE_RADIUS = (float)TILE_WIDTH / 2.0f;
    // The minimum distance before a collision occurs
    const float MIN_DISTANCE = AGENT_RADIUS + TILE_RADIUS;

    // Center position of the agent
    glm::vec2 centerAgentPos = _position + glm::vec2(AGENT_RADIUS);
    // Vector from the agent to the tile
    glm::vec2 distVec = centerAgentPos - tilePos;

    // Get the depth of the collision
    float xDepth = MIN_DISTANCE - abs(distVec.x);
    float yDepth = MIN_DISTANCE - abs(distVec.y);

    // If both the depths are > 0, then we collided
    if (xDepth > 0 && yDepth > 0) {

        // Check which collision depth is less
        if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f)) {
            // X collsion depth is smaller so we push in X direction
            if (distVec.x < 0) {
                _position.x -= xDepth;
                GameEngine::checkNan(_position);

            }
            else {
                _position.x += xDepth;
                GameEngine::checkNan(_position);

            }
        }
        else {
            // Y collsion depth is smaller so we push in X direction
            if (distVec.y < 0) {
                _position.y -= yDepth;
                GameEngine::checkNan(_position);

            }
            else {
                _position.y += yDepth;
                GameEngine::checkNan(_position);

            }
        }
    }

   // printf("Collide with tile end \n");

}