#pragma once

#include "Sprite.h"
#include "TileMap.h"
#include "Entity.h"


// Entity class is father for all entity-like son classes
// Dependant classes must implement: init

//NOTE: most attributes & methods are in Entity.h (father class)

class Mushroom : public Entity {

public:
	void init(const glm::ivec2& tileMapPos, const glm::vec2& initial_pos, TileMap* tileMap, ShaderProgram& shaderProgram);
	bool takeDamage();
	string whoAmI();

private:

};
