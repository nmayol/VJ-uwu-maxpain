#ifndef _KOOPA_INCLUDE
#define _KOOPA_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Entity.h"


// Entity class is father for all entity-like son classes
// Dependant classes must implement: init

//NOTE: most attributes & methods are in Entity.h (father class)

class Koopa : public Entity {

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	int detectPlayerCollision(glm::vec2 posPlayer, bool Falling, const glm::ivec2& size);
	bool takeDamage();
	bool canKillEnemies();
	string whoAmI();

private:
	int frames_until_respawn;
	bool shell_in_movement;
};


#endif // _KOOPA_INCLUDE

