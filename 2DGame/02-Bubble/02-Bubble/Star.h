#pragma once

#include "Sprite.h"
#include "TileMap.h"
#include "Entity.h"


// Entity class is father for all entity-like son classes
// Dependant classes must implement: init

//NOTE: most attributes & methods are in Entity.h (father class)

class Star : public Entity {

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	bool takeDamage();
	string whoAmI();

private:

};