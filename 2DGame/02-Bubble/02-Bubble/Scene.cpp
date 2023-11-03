#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Goomba.h"
#include "Koopa.h"


#define SCREEN_X 0
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 2.5
#define INIT_PLAYER_Y_TILES 12

enum PlayerColisionResult
{
	NOTHING, PLAYER_TAKES_DMG, ENTITY_TAKES_DMG, LAUNCH_SHELL
};

Scene::Scene()
{
	map = NULL;
	player = NULL;
}

Scene::~Scene()
{
	if (map != NULL)
		delete map;
	if (player != NULL)
		delete player;

	while (!enemies_in_map.empty()) delete enemies_in_map.front(), enemies_in_map.pop_front();
	while (!enemies_in_screen.empty()) delete enemies_in_screen.front(), enemies_in_screen.pop_front();
}


void Scene::init()
{
	initShaders();
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	map_sec = TileMap::createTileMap("levels/level01_sec.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize() + 128.f, INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT + 7), 8.f);
	currentTime = 0.0f;
	stopFrames = 0;

	//TODO -> FIX THIS READING FROM FILE
	Goomba* enemy_test = new Goomba();
	enemy_test->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	enemy_test->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize() + 64.f * 7 + 64.f, INIT_PLAYER_Y_TILES * map->getTileSize()));
	enemy_test->setTileMap(map);
	enemy_test->changeFacingDirection();
	enemies_in_map.push_back(enemy_test);

	enemy_test = new Goomba();
	enemy_test->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	enemy_test->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize() + 64.f * 7 + 90.f, INIT_PLAYER_Y_TILES * map->getTileSize()));
	enemy_test->setTileMap(map);
	enemy_test->changeFacingDirection();
	enemies_in_map.push_back(enemy_test);

	Koopa* enemy_test_a = new Koopa();
	enemy_test_a->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	enemy_test_a->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize() + 64.f * 4, INIT_PLAYER_Y_TILES * map->getTileSize() - 16));
	enemy_test_a->changeFacingDirection();
	enemy_test_a->setTileMap(map);

	enemy_test_a = new Koopa();
	enemy_test_a->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	enemy_test_a->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize() + 64.f * 7 + 106.f, INIT_PLAYER_Y_TILES * map->getTileSize() - 16));
	enemy_test_a->changeFacingDirection();
	enemy_test_a->setTileMap(map);

	enemies_in_map.push_back(enemy_test_a);
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	if (stopFrames > 0)
	{
		stopFrames--;
		return;
	};

	player->update(deltaTime);
	glm::vec2 posPlayer = player->getPosition();
	glm::ivec2 playerSize = player->getSize();
	bool playerIsFalling = player->isFalling();

	// Go through Enemy list and check if they are in screen
	bool stop = false;
	std::list<Entity*>::iterator it = enemies_in_map.begin();
	while (it != enemies_in_map.end() && !stop) {
		glm::ivec2 posEnemy = (*it)->getPosition();
		if ((((((int) posPlayer.x) + 192) / 64 + 2) * 64) > posEnemy.x) {
			enemies_in_screen.push_back((*it));
			it = enemies_in_map.erase(it);
		}
		else stop = true;
	}

	//update every enemy in screen
	it = enemies_in_screen.begin();
	while (it != enemies_in_screen.end()) {
		glm::vec2 posEnemy = (*it)->getPosition();
		if (posEnemy.x < posPlayer.x - 192 || (*it)->isEntityDead()) it = enemies_in_screen.erase(it);
		else {
			(*it)->update(deltaTime);
			
			if ((*it)-> isCollidable()) 
			{
				//colision with other enemies
				for (Entity* e2 : enemies_in_screen) {
					if (e2 != (*it) && e2->detectCollision(&posEnemy, (*it)->getFacingDirection(), (*it)->getSize())) {
						if ((*it)->canKillEnemies()) {
							e2->changeFacingDirection((*it)->getFacingDirection());
							e2->kill();
						}
						else if (e2->canKillEnemies()) {
							(*it)->changeFacingDirection(e2->getFacingDirection());
							(*it)->kill();
						}
						else {
							(*it)->changeFacingDirection();
							(*it)->setPosition(posEnemy);
							e2->changeFacingDirection();
						}
					}
				}

				//colision with player
				if (!player->isInvencible()) {
					int player_colision_result = (*it)->detectPlayerCollision(posPlayer, playerIsFalling, playerSize);
					if (player_colision_result == ENTITY_TAKES_DMG)
					{
						(*it)->takeDamage();
						player->applyBounce();
					}
					else if (player_colision_result == LAUNCH_SHELL) {
						(*it)->takeDamage();
						(*it)->changeFacingDirection(player->getFacingDirection());
					}
					else if (player_colision_result == PLAYER_TAKES_DMG) {
						player->takeDamage();
						stopFrames = 20;
					}
				}
			}

			++it;
		}
	}

	moveCameraifNeeded();
}

void Scene::render()
{
	glm::mat4 modelview = glm::mat4(1.0f);
	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	map->render();
	map_sec->render();
	player->render();

	for (Entity* e : enemies_in_screen) e->render();
}

void Scene::moveCameraifNeeded()
{
	float posPlayer = player->getPosition().x;
	float directionPlayer = player->getFacingDirection();
	// float altPlayer = player->getPosition().y;
	if (posPlayer < 3120 && (posPlayer - (sceneStart + float(SCREEN_WIDTH - 1)) / 3.) > 0 && (directionPlayer == 1.f)) {
		float aux = posPlayer - (float(SCREEN_WIDTH - 1)) / 3.f;
		if (aux > sceneStart) {
			sceneStart = aux;
		}
	}
	else if (directionPlayer == -1.f && posPlayer < sceneStart) {
		// make player unable to go back to last scene
		player->setPosition(glm::vec2(sceneStart, player->getPosition().y));
	}

	projection = glm::ortho(sceneStart, sceneStart + (float(SCREEN_WIDTH - 1)), float(SCREEN_HEIGHT + 7), 8.f);

}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}



