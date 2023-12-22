#include "MainGame.h"

#include <GameEngine/GameEngine.h>
#include <GameEngine/Timing.h>
#include <GameEngine/GameEngineErrors.h>
#include <GameEngine/ResourceManager.h>
#include <GameEngine/Vertex.h>


#include <algorithm>

#include <SDL/SDL.h>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

#include "Gun.h"
#include "Zombie.h"

const float HUMAN_SPEED = 1.0f;
const float ZOMBIE_SPEED = 1.3f;
const float PLAYER_SPEED = 15.0f;

MainGame::MainGame() :
    m_screenWidth(1024),
    m_screenHeight(768),
    m_gameState(GameState::PLAY),
    m_fps(0),
    m_player(nullptr),
    m_numHumansKilled(0),
    m_numZombiesKilled(0) {
    m_randomEngine.seed(time(nullptr));
}

MainGame::~MainGame() {
    // Don't forget to delete the levels!
    for (int i = 0; i < m_levels.size(); i++) {
        delete m_levels[i];
    }
    // Don't forget to delete the humans and zombies!
    for (int i = 0; i < m_humans.size(); i++) {
        delete m_humans[i];
    }
    for (int i = 0; i < m_zombies.size(); i++) {
        delete m_zombies[i];
    }
}

void MainGame::run() {

    initSystems();

    initLevel();

   GameEngine::Music music =  m_audioEngine.loadMusic("Sound/XYZ.ogg");
   music.play(-1);

    gameLoop();
}

void updateBloodParticle(GameEngine::Particle2D& particle, float deltaTime) {
    particle.position += particle.velocity * deltaTime;
    particle.color.a = (GLubyte)(particle.life * 255.0f);
}

void MainGame::initSystems() {
    // Initialize the game engine
    GameEngine::init();

    // Initialize sound, must happen affer GameEngine::init
    m_audioEngine.init();

    // Create our window
    m_window.create("ZombieGame", m_screenWidth, m_screenHeight, 0);

    // Grey background color
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

    // Set up the shaders
    initShaders();

    // Initialize our spritebatch
    m_agentSpriteBatch.init();
    m_hudSpriteBatch.init();

    //Initialize sprite font
    m_spriteFont = new GameEngine::SpriteFont("Fonts/chintzy.ttf", 64);

    // Set up the camera
    m_camera.init(m_screenWidth, m_screenHeight);
    m_hudCamera.init(m_screenWidth, m_screenHeight);
    m_hudCamera.setPosition(glm::vec2(m_screenWidth / 2, m_screenHeight / 2));

    //Initialize particle 
    m_bloodParticleBatch = new GameEngine::ParticleBatch2D();
    m_bloodParticleBatch->init(1000, 0.05f,
                               GameEngine::ResourceManager::getTexture("Textures/particle.png"), 
                                [](GameEngine::Particle2D& particle, float deltaTime) {
                                    particle.position += particle.velocity * deltaTime;
                                    particle.color.a = (GLubyte)(particle.life * 255.0f);
                                });
    m_particleEngine.addParticleBatch(m_bloodParticleBatch);

}

void MainGame::initLevel() {
    // Level 1
    m_levels.push_back(new Level("Levels/level1.txt"));
    m_currentLevel = 0;

    m_player = new Player();
    m_player->init(PLAYER_SPEED, m_levels[m_currentLevel]->getStartPlayerPos(), &m_inputManager, &m_camera, &m_bullets);

    m_humans.push_back(m_player);

    glm::vec2 pos;

    // Add all the random humans
    for (int i = 0; i < m_levels[m_currentLevel]->getNumHumans(); i++) {

        pos = randPos();

        while (true) {
            if (checkSpawnPos(m_levels[m_currentLevel]->getLevelData(), pos)) {
                m_humans.push_back(new Human);
                m_humans.back()->init(HUMAN_SPEED, pos);
                break;
            }
            pos = randPos();
        }

    }

    // Add the zombies
    const std::vector<glm::vec2>& zombiePositions = m_levels[m_currentLevel]->getZombieStartPositions();
    for (int i = 0; i < zombiePositions.size(); i++) {
        m_zombies.push_back(new Zombie);
        m_zombies.back()->init(ZOMBIE_SPEED, zombiePositions[i]);
    }

    // Set up the players guns
    const float BULLET_SPEED = 20.0f;
    m_player->addGun(new Gun("Magnum", 10, 1, 0.05f, BULLET_SPEED, 30, m_audioEngine.loadSoundEffect("Sound/shots/pistol.wav")));
    m_player->addGun(new Gun("Shotgun", 50, 12, 0.4f, BULLET_SPEED, 10, m_audioEngine.loadSoundEffect("Sound/shots/shotgun.wav")));
    m_player->addGun(new Gun("MP5", 5, 1, 0.15f, BULLET_SPEED, 20, m_audioEngine.loadSoundEffect("Sound/shots/cg1.wav")));
}

void MainGame::initShaders() {
    // Compile our color shader
    m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColor");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

void MainGame::gameLoop() {

    // Some helpful constants.
    const float DESIRED_FPS = 60.0f; // FPS the game is designed to run at
    const int MAX_PHYSICS_STEPS = 6; // Max number of physics steps per frame
    const float MS_PER_SECOND = 1000; // Number of milliseconds in a second
    const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS; // The desired frame time per frame
    const float MAX_DELTA_TIME = 1.0f; // Maximum size of deltaTime

    // Used to cap the FPS
    GameEngine::FpsLimiter fpsLimiter;
    fpsLimiter.setMaxFPS(144.0f);

    // Zoom out the camera by 3x
    const float CAMERA_SCALE = 1.0f / 3.0f;
    m_camera.setScale(CAMERA_SCALE);

    // Start our previousTicks variable
    Uint32 previousTicks = SDL_GetTicks();

    // Main loop
    while (m_gameState == GameState::PLAY) {
        fpsLimiter.begin();

        // Calculate the frameTime in milliseconds
        Uint32 newTicks = SDL_GetTicks();
        Uint32 frameTime = newTicks - previousTicks;
        previousTicks = newTicks; // Store newTicks in previousTicks so we can use it next frame
        // Get the total delta time
        float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

        checkVictory();

        m_inputManager.update();

        processInput();

        int i = 0; // This counter makes sure we don't spiral to death!

        // Loop while we still have steps to process.
        while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS) {
            // The deltaTime should be the the smaller of the totalDeltaTime and MAX_DELTA_TIME
            float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
            // Update all physics here and pass in deltaTime
            updateAgents(deltaTime);
            updateBullets(deltaTime);
            m_particleEngine.update(deltaTime);
            // Since we just took a step that is length deltaTime, subtract from totalDeltaTime
            totalDeltaTime -= deltaTime;
            // Increment our frame counter so we can limit steps to MAX_PHYSICS_STEPS
            i++;
        }
        
        // Make sure the camera is bound to the player position
        m_camera.setPosition(m_player->getPosition());
        m_camera.update();
        m_hudCamera.update();

        drawGame();

        // End the frame, limit the FPS, and get the current FPS.
        m_fps = fpsLimiter.end();
        std::cout << m_fps << std::endl;
    }
}

void MainGame::processInput() {
    SDL_Event evnt;
    //Will keep looping until there are no more events to process
    while (SDL_PollEvent(&evnt)) {
        switch (evnt.type) {
        case SDL_QUIT:
            m_gameState = GameState::EXIT;
            break;
        case SDL_MOUSEMOTION:
            m_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
            break;
        case SDL_KEYDOWN:
            m_inputManager.keyPress(evnt.key.keysym.sym);
            break;
        case SDL_KEYUP:
            m_inputManager.releaseKey(evnt.key.keysym.sym);
            break;
        case SDL_MOUSEBUTTONDOWN:
            m_inputManager.keyPress(evnt.button.button);
            break;
        case SDL_MOUSEBUTTONUP:
            m_inputManager.releaseKey(evnt.button.button);
            break;
        }
    }
}

void MainGame::drawGame() {
    // Set the base depth to 1.0
    glClearDepth(1.0);
    // Clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_textureProgram.use();

    // Draw code goes here
    glActiveTexture(GL_TEXTURE0);

    // Make sure the shader uses texture 0
    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);

    // Grab the camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Draw the level
    m_levels[m_currentLevel]->draw();

    // Begin drawing agents
    m_agentSpriteBatch.begin();

    const glm::vec2 agentDims(AGENT_RADIUS * 2.0f);

    // Draw the humans
    for (int i = 0; i < m_humans.size(); i++) {
        if (m_camera.isBoxInView(m_humans[i]->getPosition(), agentDims)) {
            m_humans[i]->draw(m_agentSpriteBatch);
        }
    }

    // Draw the zombies
    for (int i = 0; i < m_zombies.size(); i++) {
        if (m_camera.isBoxInView(m_zombies[i]->getPosition(), agentDims)) {
            m_zombies[i]->draw(m_agentSpriteBatch);
        }
    }

    // Draw the bullets
    for (int i = 0; i < m_bullets.size(); i++) {
        m_bullets[i].draw(m_agentSpriteBatch);
    }

    // End spritebatch creation
    m_agentSpriteBatch.end();

    // Render to the screen
    m_agentSpriteBatch.renderBatch();

    // Render the particles
    m_particleEngine.draw(&m_agentSpriteBatch);

    // render the hud
    drawHud();

    // Unbind the program
    m_textureProgram.unuse();

    // Swap our buffer and draw everything to the screen!
    m_window.swapBuffer();
}

void MainGame::drawHud() {
    char buffer[256];

    glm::mat4 projectionMatrix = m_hudCamera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    m_hudSpriteBatch.begin();

    sprintf_s(buffer, "Num Humans %zu", m_humans.size());
    m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(0, 0),
        glm::vec2(0.5), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255));

    sprintf_s(buffer, "Num Zombies %zu", m_zombies.size());
    m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(0, 36),
        glm::vec2(0.5), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255));

    m_hudSpriteBatch.end();
    m_hudSpriteBatch.renderBatch();
}

void MainGame::addBlood(const glm::vec2& position, int numParticles) {
    
    glm::vec2 vel(1.5f, 0.0f);
    static std::uniform_real_distribution<float> randAngle(0.0f, 360.0f);

    GameEngine::ColorRGBA8 col(255, 0, 0, 255);
    
    for (int i = 0; i < numParticles; i++) {
        m_bloodParticleBatch->addParticle(position, glm::rotate(vel, randAngle(m_randomEngine)), col, 30.0f);
    }
}

void MainGame::updateAgents(float deltaTime) {
    // Update all humans
    for (int i = 0; i < m_humans.size(); i++) {
        m_humans[i]->update(m_levels[m_currentLevel]->getLevelData(),
            m_humans,
            m_zombies,
            deltaTime);
    }

    // Update all zombies
    for (int i = 0; i < m_zombies.size(); i++) {
        m_zombies[i]->update(m_levels[m_currentLevel]->getLevelData(),
            m_humans,
            m_zombies,
            deltaTime);
    }

    // Update Zombie collisions
    for (int i = 0; i < m_zombies.size(); i++) {
        // Collide with other zombies
        for (int j = i + 1; j < m_zombies.size(); j++) {
            m_zombies[i]->collideWithAgent(m_zombies[j]);
        }
        // Collide with humans
        for (int j = 1; j < m_humans.size(); j++) {
            if (m_zombies[i]->collideWithAgent(m_humans[j])) {
                // Add the new zombie
                m_zombies[i]->m_zombieDeath.play();
                m_zombies.push_back(new Zombie);
                m_zombies.back()->init(ZOMBIE_SPEED, m_humans[j]->getPosition());
                // Delete the human
                delete m_humans[j];
                m_humans[j] = m_humans.back();
                m_humans.pop_back();
            }
        }

        // Collide with player
        if (m_zombies[i]->collideWithAgent(m_player)) {
            GameEngine::fatalError("YOU LOSE");
        }
    }

    // Update Human collisions
    for (int i = 0; i < m_humans.size(); i++) {
        // Collide with other humans
        for (int j = i + 1; j < m_humans.size(); j++) {
            m_humans[i]->collideWithAgent(m_humans[j]);
        }
    }
}

void MainGame::updateBullets(float deltaTime) {
    // Update and collide with world
    for (int i = 0; i < m_bullets.size(); ) {
        // If update returns true, the bullet collided with a wall
        if (m_bullets[i].update(m_levels[m_currentLevel]->getLevelData(), deltaTime)) {
            m_bullets[i] = m_bullets.back();
            m_bullets.pop_back();
        }
        else {
            i++;
        }
    }

    bool wasBulletRemoved;

    // Collide with humans and zombies
    for (int i = 0; i < m_bullets.size(); i++) {
        wasBulletRemoved = false;
        // Loop through zombies
        for (int j = 0; j < m_zombies.size(); ) {
            // Check collision with zombie
            if (m_bullets[i].collideWithAgent(m_zombies[j])) {
                // Add blood
                addBlood(m_bullets[i].getPosition(), 5);
                // Damage zombie, and kill it if its out of health
                if (m_zombies[j]->applyDamage(m_bullets[i].getDamage())) {
                    // If the zombie died, remove him
                    m_zombies[j]->m_zombieDeath.play();
                    delete m_zombies[j];
                    m_zombies[j] = m_zombies.back();
                    m_zombies.pop_back();
                    m_numZombiesKilled++;
                }
                else {
                    j++;
                }

                // Remove the bullet
                m_bullets[i] = m_bullets.back();
                m_bullets.pop_back();
                wasBulletRemoved = true;
                i--; // Make sure we don't skip a bullet
                // Since the bullet died, no need to loop through any more zombies
                break;
            }
            else {
                j++;
            }
        }
        // Loop through humans
        if (wasBulletRemoved == false) {
            for (int j = 1; j < m_humans.size(); ) {
                // Check collision
                if (m_bullets[i].collideWithAgent(m_humans[j])) {
                    // Add blood
                    addBlood(m_bullets[i].getPosition(), 5);
                    // Damage human, and kill it if its out of health
                    if (m_humans[j]->applyDamage(m_bullets[i].getDamage())) {
                        // If the human died, remove him
                        delete m_humans[j];
                        m_humans[j] = m_humans.back();
                        m_humans.pop_back();
                    }
                    else {
                        j++;
                    }

                    // Remove the bullet
                    m_bullets[i] = m_bullets.back();
                    m_bullets.pop_back();
                    m_numHumansKilled++;
                    i--; // Make sure we don't skip a bullet
                    // Since the bullet died, no need to loop through any more zombies
                    break;
                }
                else {
                    j++;
                }
            }
        }
    }
}

void MainGame::checkVictory() {
    // TODO: Support for multiple levels!
    // m_currentLevel++; initLevel(...);

    // If all zombies are dead we win!
    if (m_zombies.empty()) {
        // Print victory message
        std::printf("*** You win! ***\n You killed %u humans and %u zombies. There are %zu/%u civilians remaining",
            m_numHumansKilled, m_numZombiesKilled, m_humans.size() - 1, m_levels[m_currentLevel]->getNumHumans());

        // Easy way to end the game :P
        GameEngine::fatalError("");
    }
}

bool MainGame::checkSpawnPos(const std::vector<std::string>& levelData, glm::vec2 pos) {

    int a = true; 

    //checks to see if they are spawning on the same spawn pos
    for (int i = 0; i < m_humans.size(); i++) {
        if (pos == m_humans[i]->getPosition()) {
            a = false;
            break;
        }
    }

    float indexX = pos.x / TILE_WIDTH;
    float indexY = pos.y / TILE_WIDTH;

    // Checks if we are outside the world
    if (indexX < 0 || indexX >= levelData[0].size() ||
        indexY < 0 || indexY >= levelData.size()) {
        a = false;
    }

    std::string row = levelData[indexY];
    char cell = row[indexX];

    //checks if it is not an air tile
    if (cell != '.') {
        a = false;
    }

    return a;

}

glm::vec2 MainGame::randPos() {


    std::uniform_int_distribution<int> randX(2, m_levels[m_currentLevel]->getWidth() - 2);
    std::uniform_int_distribution<int> randY(2, m_levels[m_currentLevel]->getHeigth() - 2);

    float x = (randX(m_randomEngine) * TILE_WIDTH);
    float y = (randY(m_randomEngine) * TILE_WIDTH);

    return glm::vec2(x, y);
}