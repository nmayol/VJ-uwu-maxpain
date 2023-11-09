#ifndef _GOOMBA_INCLUDE
#define _GOOMBA_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Entity.h"


// Entity class is father for all entity-like son classes
// Dependant classes must implement: init

//NOTE: most attributes & methods are in Entity.h (father class)

class Goomba : public Entity {

public:
	void init(const glm::ivec2& tileMapPos, const glm::vec2& initial_pos, TileMap* tileMap, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	bool takeDamage();
	string whoAmI();

private:

	int frames_until_death;

};


#endif // _GOOMBA_INCLUDE

