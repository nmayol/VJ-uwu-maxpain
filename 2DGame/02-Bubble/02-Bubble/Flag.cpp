#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Flag.h"




//For reading Sprite
#define SPRITE_OFFSET_X (1.f / 64.f)
#define SPRITE_OFFSET_Y (1.f / 64.f)



enum FlagAnims
{
	NORMAL
};



void Flag::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{

	actualAnimation = NORMAL;

	//INIT SPRITES
	flagSpritesheet.loadFromFile("images/tileset.png", TEXTURE_PIXEL_FORMAT_RGBA);
	normalFlag = initFlagSprite(&flagSpritesheet, &shaderProgram);

	tileMapDispl = tileMapPos;
	normalFlag->setPosition(glm::vec2(float(tileMapDispl.x + posFlag.x), float(tileMapDispl.y + posFlag.y)));
	normalFlag->changeAnimation(actualAnimation);


}





Sprite* Flag::initFlagSprite(Texture* spritesheet, ShaderProgram* shaderProgram) {


	Sprite* newSprite;

	newSprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(SPRITE_OFFSET_X, SPRITE_OFFSET_Y), spritesheet, shaderProgram);
	newSprite->setNumberAnimations(2);
	newSprite->setAnimationSpeed(NORMAL, 8);
	newSprite->addKeyframe(NORMAL, glm::vec2(0.f * SPRITE_OFFSET_X, 8.f * SPRITE_OFFSET_Y));

	return newSprite;
}



void Flag::update(int deltaTime)
{
	normalFlag->setPosition(glm::vec2(float(tileMapDispl.x + posFlag.x), float(tileMapDispl.y + posFlag.y)));
	normalFlag->update(deltaTime);
}



void Flag::render()
{
	normalFlag->render();
}


void Flag::setPosition(const glm::vec2& pos)
{
	posFlag = pos;
	normalFlag->setPosition(glm::vec2(float(tileMapDispl.x + posFlag.x), float(tileMapDispl.y + posFlag.y)));
}

glm::vec2 Flag::getPosition() {
	return posFlag;
}

