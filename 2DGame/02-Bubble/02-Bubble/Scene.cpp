
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Mushroom.h"
#include "Star.h"
#include "Coin.h"


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
	while (!floating_scores.empty()) delete floating_scores.front(), floating_scores.pop_front();
	while (!power_ups.empty()) delete power_ups.front(), power_ups.pop_front();
}

void Scene::init()
{
	initShaders();
	pressed_and_released = true;
	gameState = KEEP_PLAYING;
	numLevel = 1;
	amountOfLives = 3;

	player_iface = new PlayerInterface();
	player_iface->init(texProgram);

	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
}

void Scene::initNewLevel(const int& level_id, const bool& new_game) {
	overworld = true;
	completed = false;
	numLevel = level_id;
	hasFlagBeenPicked = false;
	gameState = KEEP_PLAYING;

	if (numLevel == 1) {
		map_sec = TileMap::createTileMap("levels/level01_sec.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		readEnemies("levels/level01_enemies.txt");
		SoundController::instance()->stopAll();
		SoundController::instance()->play(LEVEL1);
	}
	else {
		map_sec2 = TileMap::createTileMap("levels/level02_sec01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		map_sec = TileMap::createTileMap("levels/level02_sec02.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		map = TileMap::createTileMap("levels/level02.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		readEnemies("levels/level02_enemies.txt");
		SoundController::instance()->stopAll();
		SoundController::instance()->play(LEVEL2);
	}

	createBlocks();
	createTeleportingTubes();
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize() - 1.f, INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
	player->reset();

	if (new_game) {
		amountOfLives = 3;
		player_iface->reset();
	}
	player_iface->changeActualLevel(level_id);
	player_iface->stopTime();


	loading_screen_frames = 120;
	timeoutFrames = 0;
	loading_screen = new loadingScreen();
	loading_screen->init(texProgram, numLevel, amountOfLives);

	createFlag();
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT + 40), 41.f);
	currentTime = 0;
	stopFrames = 0;
	sceneStart = 0.f;

	enemies_in_screen.clear();
	floating_scores.clear();
	power_ups.clear();
}

void Scene::createFlag() {
	flag = new Flag();
	flag->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	flag->setPosition(glm::vec2(197.5f * 16.f, 5.f * 16.f));
}

void Scene::createBlocks() {
	brickSet = vector<vector<Brick*>>((int)map->getMapSize().x, vector<Brick*>((int)map->getMapSize().y, NULL));
	qmBlockSet = vector<vector<QMBlock*>>((int)map->getMapSize().x, vector<QMBlock*>((int)map->getMapSize().y, NULL));
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
				qmBlockSet[i][j]->init(glm::ivec2(SCREEN_X, SCREEN_Y), false, texProgram);
				qmBlockSet[i][j]->setPosition(glm::vec2(i * map->getTileSize(), j * map->getTileSize()));
			}
			else if (qmBlockIndex[i][j] == 10) {
				qmBlockSet[i][j] = new QMBlock();
				qmBlockSet[i][j]->init(glm::ivec2(SCREEN_X, SCREEN_Y), true, texProgram);
				qmBlockSet[i][j]->setPosition(glm::vec2(i * map->getTileSize(), j * map->getTileSize()));
			}
		}
	}
}

void Scene::createTeleportingTubes()
{

	bool tubeIsHorizontal = true;
	tubeSet = vector<Tube*>(2, NULL);
	tubeSet[0] = new Tube();
	tubeSet[0]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, tubeIsHorizontal);
	tubeSet[0]->setPosition(glm::vec2(57 * 16.f, 11 * 16.f));
	tubeSet[1] = new Tube();
	tubeSet[1]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, !tubeIsHorizontal);
	tubeSet[1]->setPosition(glm::vec2(61 * 16.f, 26 * 16.f));



}


void Scene::update(int deltaTime)
{
	//CHANGE LEVEL
	if (Game::instance().getKey('1')) initNewLevel(1, false);
	if (Game::instance().getKey('2') || (numLevel == 1 && player_iface->getTime() == 0 && completed)) initNewLevel(2, false);
	else if (numLevel == 2 && player_iface->getTime() == 0 && completed) {
		gameState = GAME_COMPLETED;
		SoundController::instance()->stopAll();
		SoundController::instance()->play(GAMEOVER);
		return;
	}

	currentTime += deltaTime;
	vector<vector<int>> brickIndex = map->getBrickIndex();
	vector<vector<int>> qmBlockIndex = map->getQMBlockIndex();
	completeGameifNeeded();
	updateBricks(brickIndex, deltaTime);
	updateQMBlocks(qmBlockIndex, deltaTime);

	if (stopFrames > 0) {
		stopFrames--;
		return;
	}
	else if (dyingAnimationFrames > 0) {

		player->update(deltaTime, false, false, false, false);
		if (dyingAnimationFrames == 200) {
			SoundController::instance()->stopAll();
			SoundController::instance()->play(DEAD);
		}
		dyingAnimationFrames--;
		if (dyingAnimationFrames == 0 && timeoutFrames == 0) {
			if (amountOfLives == 0) {
				gameState = GAME_OVER; //RETURN TO MAIN SCREEN if no more lives
				SoundController::instance()->stopAll();
				SoundController::instance()->play(GAMEOVER);
				return;
			}
			player_iface->setTimeToNone();
			initNewLevel(numLevel, false); //RESTART LEVEL
		}

		return;
	}
	else if (timeoutFrames > 0) {
		timeoutFrames--;
		player_iface->update(deltaTime);
		player_iface->setScreenXandY(sceneStart, 0.f);
		if (timeoutFrames == 0) {
			if (amountOfLives == 0) {
				gameState = GAME_OVER; //RETURN TO MAIN SCREEN if no more lives
				SoundController::instance()->stopAll();
				SoundController::instance()->play(GAMEOVER);
				return;
			}
			player_iface->setTimeToNone();
			initNewLevel(numLevel, false); //RESTART LEVEL
		}
		return;
	}
	else if (loading_screen_frames > 0) {
		player_iface->update(deltaTime);
		player_iface->setScreenXandY(sceneStart, 0.f);
		loading_screen_frames--;
		if (loading_screen_frames == 0) player_iface->startTime();
		return;
	}

	player->update(deltaTime, completed, couldBeGoingUnderworld(), wantsToGoOverworld(), pickingFlag());

	sumFlagPointsIfNeeded();

	flag->update(deltaTime, pickingFlag());
	updateEnemies(deltaTime);
	player_iface->update(deltaTime);
	player_iface->setScreenXandY(sceneStart, 0.f);
	changeWorldifNeeded();
	moveCameraifNeeded();
	sumLastPoints();
	actIfMarioHasCommitedSuicide();
}


void Scene::sumFlagPointsIfNeeded() {
	if (!hasFlagBeenPicked && pickingFlag()) {
		hasFlagBeenPicked = true;
		int point_decider = int(player->getPosition().y);
		int obtained_flag_points = 0;
		if (point_decider <= 5 * 16) obtained_flag_points = 5000;
		else if (point_decider <= 7 * 16) obtained_flag_points = 2000;
		else if (point_decider <= 9 * 16) obtained_flag_points = 800;
		else if (point_decider <= 11 * 16) obtained_flag_points = 400;
		else if (point_decider <= 13 * 16) obtained_flag_points = 100;
		floating_scores.push_back(new FloatingScore(obtained_flag_points, player->getPosition(), texProgram)); //create Score
		player_iface->addToScore(obtained_flag_points);
	}
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
		if ((((((int)posPlayer.x) + 192) / 80 + 1) * 80) > posEnemy.x) {
			enemies_in_screen.push_back((*it));
			it = enemies_in_map.erase(it);
		}
		else stop = true;
	}

	//update every enemy in screen
	it = enemies_in_screen.begin();
	while (it != enemies_in_screen.end()) {
		glm::vec2 posEnemy = (*it)->getPosition();
		if (posEnemy.x < posPlayer.x - 192 || posEnemy.x > posPlayer.x + 272 || (*it)->isEntityDead()) it = enemies_in_screen.erase(it);
		else {
			(*it)->update(deltaTime);

			if ((*it)->isCollidable())
			{
				//colision with other enemies
				for (Entity* e2 : enemies_in_screen) {
					if (e2 != (*it) && e2->detectCollision(&posEnemy, (*it)->getFacingDirection(), (*it)->getSize())) {
						if ((*it)->canKillEnemies()) {
							e2->changeFacingDirection((*it)->getFacingDirection());
							FloatingScore* aux = new FloatingScore(100, e2->getPosition(), texProgram, (*it)->comboValue());
							floating_scores.push_back(aux); //create Score
							player_iface->addToScore(aux->getValueAfterCombo());
							e2->kill();
						}
						else if (e2->canKillEnemies()) {
							(*it)->changeFacingDirection(e2->getFacingDirection());
							FloatingScore* aux = new FloatingScore(100, (*it)->getPosition(), texProgram, e2->comboValue());
							floating_scores.push_back(aux); //create Score
							player_iface->addToScore(aux->getValueAfterCombo());
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
					if (player->inStarMode() && (player_colision_result != NOTHING)) {
						(*it)->changeFacingDirection(player->getFacingDirection());
						floating_scores.push_back(new FloatingScore(100, (*it)->getPosition(), texProgram)); //create Score
						player_iface->addToScore(100);
						(*it)->kill();
					}
					else if (player_colision_result == ENTITY_TAKES_DMG)
					{
						bool spawns_floating_score = (*it)->takeDamage();
						if (spawns_floating_score) {
							floating_scores.push_back(new FloatingScore(100, (*it)->getPosition(), texProgram)); //create Score
							player_iface->addToScore(100);
						}
						player->applyBounce();
					}
					else if (player_colision_result == LAUNCH_SHELL) {
						(*it)->takeDamage();
						(*it)->changeFacingDirection(player->getFacingDirection());
						floating_scores.push_back(new FloatingScore(500, (*it)->getPosition(), texProgram)); //create Score
						player_iface->addToScore(500);
						player->applyBounce();
					}
					else if (player_colision_result == PLAYER_TAKES_DMG) {
						if (player->takeDamage()) {
							//player is dead
							dyingAnimationFrames = 200;
							amountOfLives--;
						}
						stopFrames = 20;
					}
				}

			}
			++it;
		}
	}
	//render power ups if any
	for (auto it = power_ups.begin(); it != power_ups.end(); /* no increment here */) {
		if ((*it)->detectPlayerCollision(posPlayer, playerIsFalling, playerSize) != 0) {
			player->powerUp();
			stopFrames = 20;
			floating_scores.push_back(new FloatingScore(1000, (*it)->getPosition(), texProgram)); //create Score
			player_iface->addToScore(1000);

			//kill entity
			it = power_ups.erase(it);
		}
		else {
			(*it)->update(deltaTime);
			if ((*it)->getPosition().y > 280.f) it = power_ups.erase(it);
			else if ((*it)->isEntityDead()) {
				glm::vec2 finalPos = (*it)->getPosition() + glm::vec2(2.f, 20.f);
				floating_scores.push_back(new FloatingScore(200, finalPos, texProgram)); //create Score
				player_iface->addToScore(200);
				it = power_ups.erase(it);
			}
			else ++it;
		}
	}

	//render floating Scores
	for (auto it = floating_scores.begin(); it != floating_scores.end(); /* no increment here */) {
		if ((*it)->update(deltaTime)) it = floating_scores.erase(it);
		else ++it;
	}


	//time runned out
	if (player_iface->getTime() == 0 && !completed) {
		player_iface->stopTime();
		player->setMarioForm(0);
		player->takeDamage();
		amountOfLives--;
		dyingAnimationFrames = 200;
		timeoutFrames = 240;
		loading_screen->setTimeoutScreen();

	}
}


void Scene::actIfMarioHasCommitedSuicide() {
	if (player->getPosition().y > 15.1 * 16 && player->getPosition().y < 16 * 16) {
		//player is dead
		dyingAnimationFrames = 200;
		player_iface->stopTime();
		player->setMarioForm(0);
		player->takeDamage();
		amountOfLives--;
		stopFrames = 20;

	}
}

void Scene::readEnemies(const string& enemy_file)
{
	enemies_in_map.clear();
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char delimiter = ',';
	fin.open(enemy_file.c_str());
	if (!fin.is_open()) {
		// Handle file open error
		cerr << "Error opening file: " << enemy_file << endl;
		return;
	}

	//Read Enemies
	int n_Enemies;
	int enemy_type, x, y;
	getline(fin, line);
	sstream.str(line);
	sstream >> n_Enemies;

	for (int i = 0; i < n_Enemies; i++) {
		getline(fin, line);
		sstream.clear();
		sstream.str(line);
		sstream >> enemy_type >> x >> y;

		if (enemy_type == 0) {
			Goomba* enemy = new Goomba();
			enemy->init(glm::ivec2(0, 16), glm::vec2(x * 16.f, y * 16.f), map, texProgram);
			enemies_in_map.push_back(enemy);
		}
		else {
			Koopa* enemy = new Koopa();
			enemy->init(glm::ivec2(0, 16), glm::vec2(x * 16.f, y * 16.f), map, texProgram);
			enemies_in_map.push_back(enemy);
		}
	}
	fin.close();
}

void Scene::updateBricks(vector<vector<int>>& brickIndex, int deltaTime) {
	int startBlock = (int)(sceneStart / map->getTileSize());
	for (int i = startBlock; i < std::min(startBlock + 8, 210); ++i) {
		for (int j = 7; j < 12; j++) {
			if (brickIndex[i][j] == 2) { // update broken brick animation
				bool add_to_score = brickSet[i][j]->update(deltaTime, map->getBrickIndexPosition(i, j));
				if (add_to_score) player_iface->addToScore(50);
			}


		}
	}

}

void Scene::updateQMBlocks(vector<vector<int>>& qmBlockIndex, int deltaTime) {
	int startBlock = (int)(sceneStart / map->getTileSize());
	for (int i = startBlock; i < 210; ++i) {
		for (int j = 7; j < 12; j++) {
			if (qmBlockIndex[i][j] <= 3 && qmBlockIndex[i][j] >= 1 || qmBlockIndex[i][j] == 10) { // update used qm animation
				int spawn_powerup = qmBlockSet[i][j]->update(deltaTime, map->getQMBlockIndexPosition(i, j));
				if (qmBlockIndex[i][j] == 2)
					map->inactivateQMBlock(i, j);

				if (spawn_powerup == 1) { //coin
					Coin* coin_test = new Coin();
					coin_test->init(glm::ivec2(SCREEN_X, SCREEN_Y), glm::vec2(i * 16.f, (j - 1) * 16.f), map, texProgram);
					power_ups.push_back(coin_test);
					player_iface->addCoins(1);
					SoundController::instance()->play(COIN);
				}
				else if (spawn_powerup == 2 && player->getMarioForm() == 0) { //powerup mushroom
					Mushroom* mushy_test = new Mushroom();
					mushy_test->init(glm::ivec2(SCREEN_X, SCREEN_Y), glm::vec2(i * 16.f, (j - 1) * 16.f), map, texProgram);
					power_ups.push_back(mushy_test);
					SoundController::instance()->play(ITEM);
				}
				else if (spawn_powerup == 2 && player->getMarioForm() > 0) { //powerup star
					Star* star_test = new Star();
					star_test->init(glm::ivec2(SCREEN_X, SCREEN_Y), glm::vec2(i * 16.f, (j - 1) * 16.f), map, texProgram);
					power_ups.push_back(star_test);
					SoundController::instance()->play(ITEM);
				}
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

	if (loading_screen_frames > 0 || timeoutFrames > 0) {
		loading_screen->render();
		player_iface->render();
		return;
	}
	if (numLevel == 2) map_sec2->render();
	map_sec->render();
	map->render();
	player_iface->render();
	flag->render();
	player->render();
	renderTubes();
	renderBricks();

}

void Scene::renderTubes() {
	tubeSet[0]->render();
	tubeSet[1]->render();
}

void Scene::renderBricks() {
	int startBlock = (int)(sceneStart / map->getTileSize());
	vector<vector<int>> brickIndex = map->getBrickIndex();
	vector<vector<int>> qmBlockIndex = map->getQMBlockIndex();
	for (int i = startBlock; i < std::min(210, startBlock + 17); i++) {
		for (int j = 7; j < 12; j++) {
			if (brickIndex[i][j] == 1 || brickIndex[i][j] == 2) {
				brickSet[i][j]->render(currentTime);
			}
			else if (qmBlockIndex[i][j] >= 1 && qmBlockIndex[i][j] <= 3 || qmBlockIndex[i][j] == 10)
			{
				qmBlockSet[i][j]->render(currentTime);
			}
		}
	}

	for (Entity* e : enemies_in_screen) e->render();
	for (Entity* power_up : power_ups) power_up->render();
	for (FloatingScore* floating_score : floating_scores) floating_score->render();
}

void Scene::completeGameifNeeded()
{
	completed = (player->getPosition().x >= 197.5f * 16.f) && !pickingFlag();
	if (completed) {

		if (player->getPosition().x > 207.f * 16) {
			player_iface->changeTickRate(1);
			player_iface->continueTime();
		}
		else {
			player_iface->stopTime();
		}
	}
}

void Scene::sumLastPoints() {
	if (completed && player_iface->getTime() != 0 && player->getPosition().x > 207.f * 16) {
		if (player_iface->getTotalScore() % 200 == 0) SoundController::instance()->play(TICK);
		player_iface->addToScore(50);
	}
}

bool Scene::couldBeGoingUnderworld()
{
	return (player->getPosition().x > 57 * 16 && player->getPosition().x < 58 * 16. && player->getPosition().y >= 9 * 16. && player->getPosition().y <= 17 * 16.);
}


bool Scene::wantsToGoOverworld()
{
	return (player->getPosition().x >= 60 * 16 && player->getPosition().x <= 63 * 16. && player->getPosition().y >= 26 * 16. && player->getPosition().y <= 27 * 16.);
}

bool Scene::pickingFlag() {
	return (player->getPosition().x >= 197.5f * 16.f && player->getPosition().x <= 200.f * 16.f && flag->getPosition().y <= 13.f * 16.f);
}


void Scene::moveCameraifNeeded()
{
	float posPlayerX = player->getPosition().x;
	float directionPlayer = player->getFacingDirection();
	if (sceneStart < 3120 && (posPlayerX - sceneStart) > float(SCREEN_WIDTH - 1) / 3.f) {
		float aux = posPlayerX - (float(SCREEN_WIDTH - 1)) / 3.f;
		if (!overworld) {
			sceneStart = 48 * 16.f;
		}
		else if (aux > sceneStart) {
			sceneStart = aux;
		}

	}
	else if (posPlayerX < sceneStart) {
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
		limY = 10.16f * 16.f;
		limXInf = 57 * 16.f;
		limXSup = limXInf + 16;

		if (player->getPosition().y >= limY && player->getPosition().x >= limXInf && player->getPosition().x < limXSup) {
			SoundController::instance()->stopAll();
			SoundController::instance()->play(WARP);
			SoundController::instance()->play(UNDERWORLD);
			sceneStart = 48 * 16.f;
			overworld = false;
		}
	}
	else {
		limY = 15.f * 16.f;
		if (player->getPosition().y < limY) {
			SoundController::instance()->stopAll();
			if (numLevel == 1)
				SoundController::instance()->play(LEVEL1);
			else if (numLevel == 2)
			{
				SoundController::instance()->play(LEVEL2);
			}
			SoundController::instance()->play(WARP);
			sceneStart = 159 * 16.f;
			overworld = true;

		}

	}

}







void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if (!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

int Scene::actualGameState()
{
	return gameState;
}

int Scene::getFinalScore()
{
	return player_iface->getTotalScore();
}

int Scene::getFinalCoins()
{
	return player_iface->getTotalCoins();
}

int Scene::getFinalLevel()
{
	return numLevel;
}