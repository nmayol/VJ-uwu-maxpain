#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
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
	void setSceneStart(float sx);

private:
	int numLevel;
	TileMap* map;
	TileMap* map_sec;

	Player *player;
	vector<vector<Brick*>> brickSet;
	vector<vector<bool>> brickIndex;


	void readBrickSetFromFile();

	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	float sceneStart;
	

};


#endif // _SCENE_INCLUDE

