#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
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
	void setSceneStart(float sx);

	//Enemy & Other Entities handling
	list<Entity*> enemies_in_map;
	list<Entity*> enemies_in_screen;


private:
	TileMap* map;
	TileMap* map_sec;
	Player *player;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	float sceneStart;
	int stopFrames;
	

};


#endif // _SCENE_INCLUDE

