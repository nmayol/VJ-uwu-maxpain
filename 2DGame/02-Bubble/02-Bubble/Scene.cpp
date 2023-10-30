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
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	map_sec = TileMap::createTileMap("levels/level01_sec.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT+7), 8.f);
	currentTime = 0.0f;

	//DEV
	Goomba* enemy_test = new Goomba();
	enemy_test->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	enemy_test->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize() + 64.f, INIT_PLAYER_Y_TILES * map->getTileSize()));
	enemy_test->setTileMap(map);
	enemies.push_back(enemy_test);

	Koopa* enemy_test_a = new Koopa();
	enemy_test_a->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	enemy_test_a->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize() + 128.f, INIT_PLAYER_Y_TILES * map->getTileSize() - 16));
	enemy_test_a->setTileMap(map);

	enemies.push_back(enemy_test_a);
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	player->update(deltaTime);

	//DEV
	for (Entity* e : enemies) e->update(deltaTime);

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

	for (Entity* e : enemies) e->render();
}

void Scene::moveCameraifNeeded()
{
	float posPlayer = player->getPosition().x;
	float directionPlayer = player->getFacingDirection();
	// float altPlayer = player->getPosition().y;
	if (posPlayer < 3120 && (posPlayer - (sceneStart + float(SCREEN_WIDTH - 1)) / 3.) > 0 && (directionPlayer == 1.f)) {
		float aux = posPlayer - (float(SCREEN_WIDTH - 1)) / 3.;
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



