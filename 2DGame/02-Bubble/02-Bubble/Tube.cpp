
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Tube.h"




//For reading Sprite
#define SPRITE_OFFSET_X (1.f / 32.f)
#define SPRITE_OFFSET_Y (1.f / 64.f)



enum TubeAnims
{
	NORMAL
};



void Tube::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, bool horitzontal)
{

	actualAnimation = NORMAL;

	//INIT SPRITES
	tubeSpritesheet.loadFromFile("images/tileset.png", TEXTURE_PIXEL_FORMAT_RGBA);
	normalTube = initTubeSprite(&tubeSpritesheet, &shaderProgram, horitzontal);

	tileMapDispl = tileMapPos;
	normalTube->setPosition(glm::vec2(float(tileMapDispl.x + posTube.x), float(tileMapDispl.y + posTube.y)));
	normalTube->changeAnimation(actualAnimation);


}





Sprite* Tube::initTubeSprite(Texture* spritesheet, ShaderProgram* shaderProgram, bool horitzontal) {


	Sprite* newSprite;

	if (horitzontal) {
		newSprite = Sprite::createSprite(glm::ivec2(32, 16), glm::vec2(SPRITE_OFFSET_X, SPRITE_OFFSET_Y), spritesheet, shaderProgram);
		newSprite->setNumberAnimations(2);
		newSprite->setAnimationSpeed(NORMAL, 8);
		newSprite->addKeyframe(NORMAL, glm::vec2(0.f * SPRITE_OFFSET_X, 8.f * SPRITE_OFFSET_Y));
	}
	else {
		newSprite = Sprite::createSprite(glm::ivec2(16, 32), glm::vec2(SPRITE_OFFSET_Y, SPRITE_OFFSET_X), spritesheet, shaderProgram);
		newSprite->setNumberAnimations(2);
		newSprite->setAnimationSpeed(NORMAL, 8);
		newSprite->addKeyframe(NORMAL, glm::vec2(2.f * SPRITE_OFFSET_Y, 4.f * SPRITE_OFFSET_X));
	}
	return newSprite;
}






void Tube::update(int deltaTime)
{
	normalTube->setPosition(glm::vec2(float(tileMapDispl.x + posTube.x), float(tileMapDispl.y + posTube.y)));
	normalTube->update(deltaTime);

}



void Tube::render()
{
	normalTube->render();
}

//  Only callable when the brick in ENTIRE
void Tube::setPosition(const glm::vec2& pos)
{
	posTube = pos;
	normalTube->setPosition(glm::vec2(float(tileMapDispl.x + posTube.x), float(tileMapDispl.y + posTube.y)));
}

glm::vec2 Tube::getPosition() {
	return posTube;
}


