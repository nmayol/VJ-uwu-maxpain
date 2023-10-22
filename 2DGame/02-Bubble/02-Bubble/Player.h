#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime);
	void render();
	
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	
private:

	int actualAnimation;
	float actual_speed;
	float initial_jump_xspeed;
	float vertical_speed;
	float max_xspeed_allowed_jumping;
	bool bJumping;
	bool JumpedAndReleased;
	float facingDirection;

	int framesUntilSlowdown;

	glm::ivec2 tileMapDispl;
	glm::vec2 posPlayer;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	glm::ivec2 collision_box_size;

};


#endif // _PLAYER_INCLUDE


