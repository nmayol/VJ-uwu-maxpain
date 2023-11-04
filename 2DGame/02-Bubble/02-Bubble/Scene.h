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
#include "Flag.h"

#include "Entity.h"
#include <list>


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

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
	void createPlayer();


	bool completed; // True if player has picked the flag
	bool couldBeGoingUnderworld(); // True if player is in the right position to go to underworld (but we don't know if it's crouching [checked at player.cpp])
	bool wantsToGoOverworld(); // True if player is in the right position to go to overworld)
	bool pickingFlag(); // player blocked when the flag is being picked (NOT IMPLEMENTED YET)

	//Enemy & Other Entities handling
	list<Entity*> enemies_in_map;
	list<Entity*> enemies_in_screen;

private:
	int numLevel;
	bool overworld;
	TileMap* map;
	TileMap* map_sec;
	Player *player;
	Flag *flag;


	

	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	float sceneStart;
	int stopFrames;
	

	vector<vector<Brick*>> brickSet;
	vector<vector<QMBlock*>> qmBlockSet;
	vector<Tube*> tubeSet;


};


#endif // _SCENE_INCLUDE

