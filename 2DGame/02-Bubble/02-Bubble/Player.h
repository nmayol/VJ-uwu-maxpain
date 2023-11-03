#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	void setMarioForm(int formId);
	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);
	void applyBounce();

	glm::vec2 getPosition();
	glm::vec2 getPositioninTM();
	glm::ivec2 getSize();
	bool isFalling();
	bool isInvencible();
	void takeDamage();
	float getFacingDirection();

private:

	//DEV
	int pressedPCount;
	bool pressedPandReleased;

	//OTHERS
	int actualAnimation;
	int actualForm;
	float actual_speed;
	float initial_jump_xspeed;
	float vertical_speed;
	float max_xspeed_allowed_jumping;
	bool bJumping;
	bool JumpedAndReleased;
	float facingDirection;

	int framesUntilSlowdown;
	int invencibleFrames;

	glm::ivec2 tileMapDispl;
	glm::vec2 posPlayer;
	int jumpAngle, startY;
	Sprite* sprite;

	TileMap* map;
	glm::ivec2 collision_box_size;

	//Diferent Mario Sprites & SpriteFiles
	Texture smallMarioSpritesheet;
	Texture normalMarioSpritesheet;
	Sprite* smallMarioSprite;
	Sprite* normalMarioSprite;
	Sprite* fireMarioSprite;
	Sprite* initSmallMarioSprite(Texture* spritesheet, ShaderProgram* shaderProgram);
	Sprite* initNormalMarioSprite(float baseSpriteRow, Texture* spritesheet, ShaderProgram* shaderProgram);
	

};


#endif // _PLAYER_INCLUDE

