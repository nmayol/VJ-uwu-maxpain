#ifndef _QMBLOCK_INCLUDE
#define _QMBLOCK_INCLUDE


#include "Sprite.h"



// QMBlock is basically a Sprite that represents the QMBlock. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class QMBlock
{

public:
	void init(const glm::ivec2& tileMapPos, const bool& contains_powerup, ShaderProgram& shaderProgram);
	int update(int deltaTime, bool justUsed);
	void render(int deltaTime);
	void setPosition(const glm::vec2& pos);
	glm::vec2 getPosition();
	float getFacingDirection();

private:


	//OTHERS
	bool contains_power_up;
	int actualAnimation;
	float vys; float vxs;
	float vyi; float vxi;

	float facingDirection;

	int frames_from_breaking;

	glm::ivec2 tileMapDispl;
	glm::vec2 posQMBlock;


	//Diferent QMBlock Sprites & SpriteFiles
	Texture QMBlockSpritesheet;
	Texture normalQMBlockSpritesheet;
	Sprite* normalQMBlock;
	


	Sprite* initQMBlockSprite(Texture* spritesheet, ShaderProgram* shaderProgram);
	
};


#endif // _QMBlock_INCLUDE