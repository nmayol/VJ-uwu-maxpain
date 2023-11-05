
#include <iostream>
#include <fstream>
#include <sstream>
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
	player_iface = NULL;
}

Scene::~Scene()
{
	if (map != NULL)
		delete map;
	if (player != NULL)
		delete player;

	if (player_iface != NULL)
		delete player_iface;

	while (!enemies_in_map.empty()) delete enemies_in_map.front(), enemies_in_map.pop_front();
	while (!enemies_in_screen.empty()) delete enemies_in_screen.front(), enemies_in_screen.pop_front();
}




void Scene::init()
{
	initShaders();
	numLevel = 1;

	overworld = true;
	completed = false;
	map_sec = TileMap::createTileMap("levels/level01_sec.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	
	createBlocks();
	createTeleportingTubes();
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize() + 128.f, INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);

	player_iface = new PlayerInterface();
	player_iface->init(texProgram);
	
	createPlayer();
	createFlag();

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT+40), 41.f);
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


void Scene::createFlag() {
	flag = new Flag();
	flag->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	flag->setPosition(glm::vec2(197.5f * 16.f, 5.f * 16.f));
}


void Scene::createPlayer() {
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
}

void Scene::createBlocks() {
	brickSet = vector<vector<Brick*>>(map->getMapSize().x, vector<Brick*>(map->getMapSize().y, NULL));
	qmBlockSet = vector<vector<QMBlock*>>(map->getMapSize().x, vector<QMBlock*>(map->getMapSize().y, NULL));
	vector<vector<int>> brickIndex = map->getBrickIndex();
	vector<vector<int>> qmBlockIndex = map->getQMBlockIndex();
	for (int i = 0; i < map->getMapSize().x; i++) {
		for (int j = 7; j < 12; j++) {
			brickSet[i][j] = new Brick();
			if (brickIndex[i][j] == 1) {
				brickSet[i][j] = new Brick();
				brickSet[i][j]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
				brickSet[i][j]->setPosition(glm::vec2(i * map->getTileSize(), j * map->getTileSize()));
			}
			else if (qmBlockIndex[i][j] == 1) {
				qmBlockSet[i][j] = new QMBlock();
				qmBlockSet[i][j]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
				qmBlockSet[i][j]->setPosition(glm::vec2(i * map->getTileSize(), j * map->getTileSize()));
			}
		}
	}
}

void Scene::createTeleportingTubes()
{
	if (numLevel == 1) {
		bool tubeIsHorizontal = true;
		tubeSet = vector<Tube*>(2, NULL);
		tubeSet[0] = new Tube();
		tubeSet[0]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, tubeIsHorizontal);
		tubeSet[0]->setPosition(glm::vec2(57 * 16.f, 11 * 16.f));
		tubeSet[1] = new Tube();
		tubeSet[1]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, !tubeIsHorizontal);
		tubeSet[1]->setPosition(glm::vec2(61 * 16.f, 26 * 16.f));

	}

}


void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	vector<vector<int>> brickIndex = map->getBrickIndex();
	vector<vector<int>> qmBlockIndex = map->getQMBlockIndex();
	completeGameifNeeded();
	player->update(deltaTime, completed, couldBeGoingUnderworld(), wantsToGoOverworld(), pickingFlag());
	updateBricks(brickIndex, deltaTime);
	updateQMBlocks(qmBlockIndex, deltaTime);
	flag->update(deltaTime,pickingFlag());
	if (stopFrames > 0)
	{
		stopFrames--;
		return;
	};

	updateEnemies(deltaTime);
	player_iface->update(deltaTime);
	player_iface->setScreenXandY(sceneStart, 0.f);
	changeWorldifNeeded();
	moveCameraifNeeded();
}




void Scene::updateEnemies(int deltaTime) {
	glm::vec2 posPlayer = player->getPosition();
	glm::ivec2 playerSize = player->getSize();
	bool playerIsFalling = player->isFalling();

	// Go through Enemy list and check if they are in screen
	bool stop = false;
	std::list<Entity*>::iterator it = enemies_in_map.begin();
	while (it != enemies_in_map.end() && !stop) {
		glm::ivec2 posEnemy = (*it)->getPosition();
		if ((((((int)posPlayer.x) + 192) / 64 + 2) * 64) > posEnemy.x) {
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

			if ((*it)->isCollidable())
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
}

void Scene::updateBricks(vector<vector<int>>& brickIndex, int deltaTime) {
	int startBlock = (sceneStart / map->getTileSize());
	for (int i = startBlock; i < min(startBlock + 8,210); ++i) {
		for (int j = 7; j < 12; j++) {
			if (brickIndex[i][j] == 2) { // update broken brick animation
				brickSet[i][j]->update(deltaTime, map->getBrickIndexPosition(i, j));
			}


		}
	}

}

void Scene::updateQMBlocks(vector<vector<int>>& qmBlockIndex, int deltaTime) {
	int startBlock = (sceneStart / map->getTileSize());
	for (int i = startBlock; i < 210; ++i) {
		for (int j = 7; j < 12; j++) {
			if (qmBlockIndex[i][j] <= 3 && qmBlockIndex[i][j] >= 1) { // update used qm animation
				qmBlockSet[i][j]->update(deltaTime, map->getQMBlockIndexPosition(i, j));
				if (qmBlockIndex[i][j] == 2)
					map->inactivateQMBlock(i, j);
			}
		}
	}
}

void Scene::render()
{
	glm::mat4 modelview = glm::mat4(1.0f);
	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);	
	map_sec->render();
	map->render();
	player_iface->render();
	flag->render();
	player->render();
	renderTubes();
	renderBricks();
		
}

void Scene::renderTubes() {
	if (numLevel == 1) {
		tubeSet[0]->render();
		tubeSet[1]->render();
	}
	else if (numLevel == 2) {
		;
	}
}

void Scene::renderBricks() {
	int startBlock = (sceneStart / map->getTileSize());
	vector<vector<int>> brickIndex = map->getBrickIndex();
	vector<vector<int>> qmBlockIndex = map->getQMBlockIndex();
	for (int i = startBlock; i < min(210, startBlock + 17); i++) {
		for (int j = 7; j < 12; j++) {
			if (brickIndex[i][j] == 1 || brickIndex[i][j] == 2) {
				brickSet[i][j]->render(currentTime);
			}
			else if (qmBlockIndex[i][j] >= 1 && qmBlockIndex[i][j] <= 3)
			{
				qmBlockSet[i][j]->render(currentTime);
			}
		}
	}

	for (Entity* e : enemies_in_screen) e->render();
}

void Scene::completeGameifNeeded()
{
	completed = (player->getPosition().x >= 197.5f * 16.f) && !pickingFlag();
}

bool Scene::couldBeGoingUnderworld()
{
	return (player->getPosition().x > 57 * 16 && player->getPosition().x < 58 * 16. && player->getPosition().y >= 9 * 16. && player->getPosition().y <= 17 * 16.);
}


bool Scene::wantsToGoOverworld()
{
	return (player->getPosition().x >= 60 * 16 && player->getPosition().x < 62 * 16. && player->getPosition().y >= 26 * 16. && player->getPosition().y <= 27 * 16.);
}

bool Scene::pickingFlag() {
	return (player->getPosition().x >= 197.5f * 16.f && player->getPosition().x <= 200.f * 16.f && flag->getPosition().y <= 13.f * 16.f);
}


void Scene::moveCameraifNeeded()
{



	float posPlayerX = player->getPosition().x;
	float directionPlayer = player->getFacingDirection();
	if (sceneStart < 3120 && (posPlayerX - (sceneStart + float(SCREEN_WIDTH - 1)) / 3.) > 0 && (directionPlayer == 1.f)) {
		float aux = posPlayerX - (float(SCREEN_WIDTH - 1)) / 3.;
		if (!overworld) {
			sceneStart = 48 * 16.f;
		}
		else if (aux > sceneStart) {
			sceneStart = aux;
		}
		
	}
	else if (directionPlayer == -1.f && posPlayerX < sceneStart) {
		// make player unable to go back to last scene
		player->setPosition(glm::vec2(sceneStart, player->getPosition().y));

	}

	float down, up, left, right;
	if (overworld) {
		left = sceneStart;
		right = sceneStart + (float(SCREEN_WIDTH - 1));
		down = float(SCREEN_HEIGHT + 40.);
		up = 41.;
	}
	else {
		left = 48 * 16.f;
		right = 48 * 16.f + (float(SCREEN_WIDTH - 1));
		down = 16 * 16 + float(SCREEN_HEIGHT);
		up = 16 * 16 + 1;
	}

	projection = glm::ortho(left, right, down, up);
	

}

// Just checks player is on the point and changes sceneStart, overworld. Then moveCameraifNeeded() will do the rest.
void Scene::changeWorldifNeeded() {
	float limY, limXInf, limXSup;
	if (numLevel < 1) return; // text scenes don't need to change this.

	if (overworld) {
		if (numLevel == 1) {
			limY = 10.16f * 16.f;
			limXInf = 57 * 16.f;
			limXSup = limXInf + 16;
		}
		if (player->getPosition().y >= limY && player->getPosition().x >= limXInf && player->getPosition().x < limXSup) {
			sceneStart = 48 * 16.f;
			overworld = false;
		}
	}
	else {
		if (numLevel == 1) {
			limY = 26.f * 16.f;
			limXInf = 61 * 16.f;
			limXSup = limXInf + 16;
		}
		if (player->getPosition().y >= limY && player->getPosition().x >= limXInf && player->getPosition().x < limXSup) {
			sceneStart = 159 * 16.f;
			overworld = true;
		}

	}

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