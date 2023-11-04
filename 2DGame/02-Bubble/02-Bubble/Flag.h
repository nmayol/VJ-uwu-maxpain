#ifndef _FLAG_INCLUDE
#define _FLAG_INCLUDE


#include "Sprite.h"



// Flag is basically a Sprite that represents the Flag. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Flag
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime, bool pickingFlag);
	void render();
	void setPosition(const glm::vec2& pos);
	glm::vec2 getPosition();

private:


	//OTHERS
	int actualAnimation;



	glm::ivec2 tileMapDispl;
	glm::vec2 posFlag;


	//Diferent Flag Sprites & SpriteFiles
	Texture flagSpritesheet;
	Sprite* normalFlag;

	Sprite* initFlagSprite(Texture* spritesheet, ShaderProgram* shaderProgram);

};


#endif // _FLAG_INCLUDE