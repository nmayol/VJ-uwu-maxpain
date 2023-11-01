#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "QMBlock.h"
#include "Game.h"


// <- and -> Physics Values from ORIGINAL GAME
#define INITIAL_SPEED 20.f
#define ALPHA 3.14159265f / 2.2f
#define BETA 3.14159265f / 2.5f
#define GRAVITY 9.8f

//For reading Sprite
#define SPRITE_OFFSET_X (1.f / 32.f)
#define LITTLE_SPRITE_OFFSET_X (1.f / 64.f)
#define SPRITE_OFFSET_Y (1.f / 2.f)
#define LITTLE_SPRITE_OFFSET_Y (1.f / 4.f)


enum QMBlockAnims
{
	NORMAL, USED
};



void QMBlock::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{


	frames_from_breaking = -1;
	actualAnimation = NORMAL;

	//INIT SPRITES
	QMBlockSpritesheet.loadFromFile("images/brick.png", TEXTURE_PIXEL_FORMAT_RGBA);
	normalQMBlock = initQMBlockSprite(&QMBlockSpritesheet, &shaderProgram);
	
	tileMapDispl = tileMapPos;

	normalQMBlock->setPosition(glm::vec2(float(tileMapDispl.x + posQMBlock.x), float(tileMapDispl.y + posQMBlock.y)));
	normalQMBlock->changeAnimation(actualAnimation);


}


Sprite* QMBlock::initQMBlockSprite(Texture* spritesheet, ShaderProgram* shaderProgram) {

	Sprite* newSprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(SPRITE_OFFSET_X, SPRITE_OFFSET_Y), spritesheet, shaderProgram);
	newSprite->setNumberAnimations(2);
	newSprite->setAnimationSpeed(NORMAL, 8);
	newSprite->addKeyframe(NORMAL, glm::vec2(0.f, 0.f));


	return newSprite;
}






void QMBlock::update(int deltaTime, bool justBroken)
{
		// actualAnimation = BROKEN;
	normalQMBlock->update(deltaTime);
	if (normalQMBlock->animation() != actualAnimation) normalQMBlock->changeAnimation(actualAnimation);
	normalQMBlock->setPosition(glm::vec2(float(tileMapDispl.x + posQMBlock.x), float(tileMapDispl.y + posQMBlock.y)));
}



void QMBlock::render(int deltaTime)
{
	if (actualAnimation == NORMAL) {
		normalQMBlock->render();
	}
	else {
		/*normalQMBlock->renderBroken(deltaTime);*/
	}
}

//  Only callable when the brick in ENTIRE
void QMBlock::setPosition(const glm::vec2& pos)
{
	posQMBlock = pos;
	normalQMBlock->setPosition(glm::vec2(float(tileMapDispl.x + posQMBlock.x), float(tileMapDispl.y + posQMBlock.y)));
}

glm::vec2 QMBlock::getPosition() {
	return posQMBlock;
}

float QMBlock::getFacingDirection() {
	return facingDirection;
}

