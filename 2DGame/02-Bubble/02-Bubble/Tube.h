#ifndef _TUBE_INCLUDE
#define _TUBE_INCLUDE


#include "Sprite.h"



// Tube is basically a Sprite that represents the Tube. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Tube
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, bool horizontal);
	void update(int deltaTime);
	void render();
	void setPosition(const glm::vec2& pos);
	glm::vec2 getPosition();

private:


	//OTHERS
	int actualAnimation;
	
	
	//bool horizontal;

	glm::ivec2 tileMapDispl;
	glm::vec2 posTube;


	//Diferent Tube Sprites & SpriteFiles
	Texture tubeSpritesheet;
	Sprite* normalTube;



	Sprite* initTubeSprite(Texture* spritesheet, ShaderProgram* shaderProgram, bool horitzontal);

};


#endif // _TUBE_INCLUDE