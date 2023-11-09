#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Brick.h"
#include "QMBlock.h"
#include "Tube.h"
#include "PlayerInterface.h"
#include "Flag.h"
#include "Entity.h"
#include "loadingScreen.h"
#include "FloatingScore.h"
#include <list>


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void initNewLevel(const int& level_id, const bool& new_game);
	void update(int deltaTime);
	void render();

	int actualGameState();
	int getFinalScore();
	int getFinalCoins();
	int getFinalLevel();

private:
	void initShaders();
	void moveCameraifNeeded();
	void updateBricks(vector<vector<int>>& brickIndex, int deltaTime);
	void updateQMBlocks(vector<vector<int>>& qmBlockIndex, int deltaTime);
	void updateEnemies(int deltaTime);
	void renderBricks();
	void completeGameifNeeded();
	void changeWorldifNeeded();
	void renderTubes();
	void createTeleportingTubes();
	void createBlocks();
	void createFlag();
	void actIfMarioHasCommitedSuicide();
	void sumLastPoints();
	void readEnemies(const string& enemy_file);

	bool pressed_and_released;
	bool completed; // True if player has picked the flag
	bool couldBeGoingUnderworld(); // True if player is in the right position to go to underworld (but we don't know if it's crouching [checked at player.cpp])
	bool wantsToGoOverworld(); // True if player is in the right position to go to overworld)
	bool pickingFlag(); // player blocked when the flag is being picked (NOT IMPLEMENTED YET)
	void sumFlagPointsIfNeeded();

	//Enemy & Other Entities handling
	list<Entity*> enemies_in_map;
	list<Entity*> enemies_in_screen;
	list<Entity*> power_ups;
	list<FloatingScore*> floating_scores;

private:
	int gameState;
	int numLevel;
	int amountOfLives;
	bool overworld;
	TileMap* map;
	TileMap* map_sec;
	TileMap* map_sec2;

	Player *player;
	Flag *flag;

	int loading_screen_frames;
	int dyingAnimationFrames;
	int timeoutFrames;
	PlayerInterface* player_iface;
	loadingScreen* loading_screen;

	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	float sceneStart;
	int stopFrames;
	bool hasFlagBeenPicked;
	

	vector<vector<Brick*>> brickSet;
	vector<vector<QMBlock*>> qmBlockSet;
	vector<Tube*> tubeSet;


};

enum screenUpdateResult
{
	KEEP_PLAYING, GAME_OVER, GAME_COMPLETED
};

#endif // _SCENE_INCLUDE

