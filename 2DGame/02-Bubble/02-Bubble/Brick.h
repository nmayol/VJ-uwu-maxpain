#ifndef _BRICK_INCLUDE
#define _BRICK_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// brick is basically a Sprite that represents the brick. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Brick
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	void setBrickForm(int formId);
	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);
	glm::vec2 getPosition();
	glm::vec2 getPositioninTM();
	float getFacingDirection();

private:


	//OTHERS
	int actualAnimation;
	int actualForm;
	float actual_speed;
	float initial_jump_xspeed;
	float vertical_speed;
	float max_xspeed_allowed_jumping;

	float facingDirection;

	int framesUntilSlowdown;

	glm::ivec2 tileMapDispl;
	glm::vec2 posBrick;
	int jumpAngle, startY;
	Sprite* sprite;

	TileMap* map;
	glm::ivec2 collision_box_size;

	//Diferent Brick Sprites & SpriteFiles
	Texture entireBrickSpritesheet;
	Texture normalBrickSpritesheet;
	Sprite* entireBrickSprite;
	Sprite* normalBrickSprite;
	Sprite* fireBrickSprite;
	Sprite* initEntireBrickSprite(Texture* spritesheet, ShaderProgram* shaderProgram);
	Sprite* initNormalBrickSprite(float baseSpriteRow, Texture* spritesheet, ShaderProgram* shaderProgram);
	

};


#endif // _BRICK_INCLUDE
