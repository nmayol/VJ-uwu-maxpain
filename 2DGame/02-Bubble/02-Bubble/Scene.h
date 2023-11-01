#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Brick.h"



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

private:
	int numLevel;
	TileMap* map;
	TileMap* map_sec;

	Player *player;
	vector<vector<bool>> brickIndex;



	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	float sceneStart;

	vector<vector<Brick*>> brickSet;


};


#endif // _SCENE_INCLUDE

