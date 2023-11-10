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
	virtual void init(const glm::ivec2& tileMapPos, const glm::vec2& initial_pos, TileMap* tileMap, ShaderProgram& shaderProgram) = 0;
	virtual bool takeDamage() = 0;
	virtual string whoAmI() = 0; // for debugging

	//Implemented Methods
	virtual void update(int deltaTime);
	void render();

	void setPosition(const glm::vec2& pos);
	glm::vec2 getPosition();
	glm::vec2 getPositioninTM();

	bool isCollidable();
	float getFacingDirection();
	void changeFacingDirection();
	void changeFacingDirection(const float& new_direction);

	glm::ivec2 getSize();
	bool detectCollision(glm::vec2* posEntity, float facingDirection, const glm::ivec2& size);
	virtual int detectPlayerCollision(glm::vec2 posPlayer, bool Falling, const glm::ivec2& size);
	void kill();
	bool isEntityDead();
	virtual bool canKillEnemies();

	int comboValue();

protected:
	bool is_collidable;
	bool is_dead;
	float vertical_speed;
	float base_vertical_speed;
	float gravity;
	float horitzontal_speed;
	float facingDirection;
	int kill_frames;
	int combo = 0;

	glm::ivec2 tileMapDispl;
	glm::vec2 posEntity;
	Sprite* sprite;

	TileMap* map;
	glm::ivec2 collision_box_size;
	Texture entitySpritesheet;

};

#endif // _ENTITY_INCLUDE

