#ifndef _BRICK_INCLUDE
#define _BRICK_INCLUDE


#include "Sprite.h"



// brick is basically a Sprite that represents the brick. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Brick
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime, bool justBroken);
	void render(int deltaTime);

	void setBrickForm(int formId);
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
	float vys; float vxs;
	float vyi; float vxi;
	float max_xspeed_allowed_jumping;

	float facingDirection;

	int framesUntilSlowdown;
	int frames_from_breaking;

	glm::ivec2 tileMapDispl;
	glm::vec2 posBrick;
	glm::vec2 posBrokenBrickSL;
	glm::vec2 posBrokenBrickSR;
	glm::vec2 posBrokenBrickIL;
	glm::vec2 posBrokenBrickIR;

	int jumpAngle, startY;
	Sprite* sprite;
	glm::ivec2 collision_box_size;

	//Diferent Brick Sprites & SpriteFiles
	Texture brickSpritesheet;
	Texture normalBrickSpritesheet;
	Sprite* entireBrickSprite;
	Sprite* brokenBrickSpriteSL;
	Sprite* brokenBrickSpriteSR;
	Sprite* brokenBrickSpriteIL;
	Sprite* brokenBrickSpriteIR;


	Sprite* initEntireBrickSprite(Texture* spritesheet, ShaderProgram* shaderProgram);
	Sprite* initBrokenBrickSprite(Texture* spritesheet, ShaderProgram* shaderProgram, int x, int y);

};


#endif // _BRICK_INCLUDE