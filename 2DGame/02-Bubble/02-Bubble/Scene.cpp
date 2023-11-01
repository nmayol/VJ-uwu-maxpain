
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 0
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 2.5
#define INIT_PLAYER_Y_TILES 12

using namespace std;


Scene::Scene()
{
	map = NULL;
	player = NULL;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
}




void Scene::init()
{
	initShaders();
	numLevel = 1;
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	brickSet = vector<vector<Brick*>>(map->getMapSize().x, vector<Brick*>(map->getMapSize().y, NULL));
	vector<vector<int>> brickIndex = map->getBrickIndex();
	for (int i = 0; i < map->getMapSize().x; i++) {
		for (int j = 5; j < 10; j++) {
			brickSet[i][j] = new Brick();
			if (brickIndex[i][j] == 1) {
				brickSet[i][j] = new Brick();
				brickSet[i][j]->init(glm::ivec2(SCREEN_X,SCREEN_Y), texProgram);
				brickSet[i][j]->setPosition(glm::vec2(i * map->getTileSize(), j * map->getTileSize()));
			}
		}
	}


	map_sec = TileMap::createTileMap("levels/level01_sec.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);

	
	

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT+7), 8.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	vector<vector<int>> brickIndex = map->getBrickIndex();
	player->update(deltaTime);	
	updateBricks(brickIndex, deltaTime);	
	moveCameraifNeeded();
}


void Scene::updateBricks(vector<vector<int>>& brickIndex, int deltaTime) {
	int startBlock = (sceneStart / map->getTileSize());

	for (int i = startBlock; i < min(startBlock + 8,210); ++i) {
		for (int j = 5; j < 10; j++) {
			if (brickIndex[i][j] == 2) { // update broken brick animation
				brickSet[i][j]->update(deltaTime, map->getBrickIndexPosition(i, j));
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
	map->render();
	map_sec->render();
	player->render();
	renderBricks();
		
}

void Scene::renderBricks() {
	int startBlock = (sceneStart / map->getTileSize());
	vector<vector<int>> brickIndex = map->getBrickIndex();
	for (int i = startBlock; i < min(210, startBlock + 17); i++) {
		for (int j = 5; j < 10; j++) {
			if (brickIndex[i][j] == 1 || brickIndex[i][j] == 2) {
				brickSet[i][j]->render(currentTime);
			}
		}
	}
}



void Scene::moveCameraifNeeded()
{
	float posPlayerX = player->getPosition().x;
	float directionPlayer = player->getFacingDirection();
	if (sceneStart < 3120 && (posPlayerX - (sceneStart + float(SCREEN_WIDTH - 1)) / 3.) > 0 && (directionPlayer == 1.f)) {
		float aux = posPlayerX - (float(SCREEN_WIDTH - 1)) / 3.;
		if (aux > sceneStart) {
			sceneStart = aux;
		}
	}
	else if (directionPlayer == -1.f && posPlayerX < sceneStart) {
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


