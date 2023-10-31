#ifndef _ENTITY_INCLUDE
#define _ENTITY_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Entity class is an Abstract class father for all entity-like son classes
// Dependant classes must implement: init


class Entity
{

public:

	//Abstract
	virtual void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram) = 0;

	//Implemented Methods
	void update(int deltaTime);
	void render();
	void setTileMap(TileMap* tileMap);

	void setPosition(const glm::vec2& pos);
	glm::vec2 getPosition();
	glm::vec2 getPositioninTM();

	float getFacingDirection();
	void changeFacingDirection();

	glm::ivec2 getSize();
	bool detectCollision(glm::vec2* posEntity, float facingDirection, const glm::ivec2& size);
	bool isEntityDead();

	virtual string whoAmI() = 0;

protected:
	bool is_dead;
	float vertical_speed;
	float horitzontal_speed;
	float facingDirection;

	glm::ivec2 tileMapDispl;
	glm::vec2 posEntity;
	Sprite* sprite;

	TileMap* map;
	glm::ivec2 collision_box_size;
	Texture entitySpritesheet;

};

#endif // _ENTITY_INCLUDE

