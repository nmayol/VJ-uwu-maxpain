#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Brick.h"
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


enum BrickAnims
{
	ENTIRE, BROKEN
};



void Brick::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{

	
	frames_from_breaking = -1;	
	actualAnimation = ENTIRE;
	
	//INIT SPRITES
	brickSpritesheet.loadFromFile("images/Brick.png",TEXTURE_PIXEL_FORMAT_RGBA);
	brickSpritesheet.setMagFilter(GL_NEAREST);
	entireBrickSprite = initEntireBrickSprite(&brickSpritesheet, &shaderProgram);
	brokenBrickSpriteSL = initBrokenBrickSprite(&brickSpritesheet, &shaderProgram, 0, 0);
	brokenBrickSpriteSR = initBrokenBrickSprite(&brickSpritesheet, &shaderProgram, 0, 1);
	brokenBrickSpriteIR = initBrokenBrickSprite(&brickSpritesheet, &shaderProgram, 1, 1);
	brokenBrickSpriteIL = initBrokenBrickSprite(&brickSpritesheet, &shaderProgram, 1, 0);
	brokenBrickSpriteIL->changeDirection(0);
	brokenBrickSpriteSL->changeDirection(0);
	

	
	tileMapDispl = tileMapPos;
	
	entireBrickSprite->setPosition(glm::vec2(float(tileMapDispl.x + posBrick.x), float(tileMapDispl.y + posBrick.y)));
	entireBrickSprite->changeAnimation(actualAnimation);
	

}

//INIT SMALL Brick SPRITE_SHEET
Sprite* Brick::initEntireBrickSprite(Texture* spritesheet, ShaderProgram* shaderProgram) {

	Sprite* newSprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(SPRITE_OFFSET_X, SPRITE_OFFSET_Y), spritesheet, shaderProgram);
	newSprite->setNumberAnimations(1);
	newSprite->setAnimationSpeed(ENTIRE, 8);
	newSprite->addKeyframe(ENTIRE, glm::vec2(23.f*SPRITE_OFFSET_X, 0.f));
	

	return newSprite;
}


//INIT SMALL Brick SPRITE_SHEET
Sprite* Brick::initBrokenBrickSprite(Texture* spritesheet, ShaderProgram* shaderProgram, int x, int y) { // x = 0 -> left, x = 1 -> right; y = 0 -> superior, y = 1 -> inferior
	
	Sprite* newSprite = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(LITTLE_SPRITE_OFFSET_X, LITTLE_SPRITE_OFFSET_Y), spritesheet, shaderProgram);
	newSprite->setNumberAnimations(2);

	newSprite->setAnimationSpeed(BROKEN, 8);
	float spriteX = (50.f + float(x)) * LITTLE_SPRITE_OFFSET_X;
	float spriteY = float(y) * LITTLE_SPRITE_OFFSET_Y;
	newSprite->addKeyframe(BROKEN, glm::vec2(spriteX,spriteY));


	return newSprite;
}





void Brick::update(int deltaTime, bool justBroken)
{
	
	if (justBroken && frames_from_breaking == -1) {
		actualAnimation = BROKEN;
		brokenBrickSpriteIL->changeAnimation(actualAnimation);
		brokenBrickSpriteIR->changeAnimation(actualAnimation);
		brokenBrickSpriteSL->changeAnimation(actualAnimation);
		brokenBrickSpriteSR->changeAnimation(actualAnimation);
		frames_from_breaking = 0;

		// Init brick falling
		posBrokenBrickIL = posBrick + glm::vec2(0, 8);
		posBrokenBrickIR = posBrick + glm::vec2(8, 8);
		posBrokenBrickSL = posBrick + glm::vec2(0, 0);
		posBrokenBrickSR = posBrick + glm::vec2(8, 0);
		vys = -INITIAL_SPEED * sin(ALPHA);
		vxs = INITIAL_SPEED * cos(ALPHA);
		vxi = INITIAL_SPEED * cos(BETA);
		vyi = -INITIAL_SPEED * sin(BETA);

		SoundController::instance()->play(BREAK);
	}
	if (frames_from_breaking >= 0) {

		brokenBrickSpriteSL->update(deltaTime);
		brokenBrickSpriteSR->update(deltaTime);
		brokenBrickSpriteIR->update(deltaTime);
		brokenBrickSpriteIL->update(deltaTime);

		float t = deltaTime / 100.f;
		
		posBrokenBrickSR = posBrokenBrickSR + glm::vec2(vxs * t, vys * t - GRAVITY/2. * t * t);
		posBrokenBrickSL = posBrokenBrickSL + glm::vec2(-vxs * t, vys * t - GRAVITY/2. * t * t);
		posBrokenBrickIR = posBrokenBrickIR + glm::vec2(vxi * t, vyi * t - GRAVITY/2. * t * t);
		posBrokenBrickIL = posBrokenBrickIL + glm::vec2(-vxi * t, vyi*t - GRAVITY/2. * t * t);
		
		vys += GRAVITY * t;
		vyi += GRAVITY * t;
		
		brokenBrickSpriteSL->setPosition(posBrokenBrickSL);
		brokenBrickSpriteSR->setPosition(posBrokenBrickSR);
		brokenBrickSpriteIR->setPosition(posBrokenBrickIR);
		brokenBrickSpriteIL->setPosition(posBrokenBrickIL);

		
		++frames_from_breaking;
	}
	else {
		entireBrickSprite->update(deltaTime);
		if (entireBrickSprite->animation() != actualAnimation) entireBrickSprite->changeAnimation(actualAnimation);
		entireBrickSprite->setPosition(glm::vec2(float(tileMapDispl.x + posBrick.x), float(tileMapDispl.y + posBrick.y)));

	}

}



void Brick::render(int deltaTime)
{
	if (actualAnimation == ENTIRE) {
		entireBrickSprite->render();
	}
	else {
		brokenBrickSpriteIL->renderBroken(deltaTime);
		brokenBrickSpriteIR->renderBroken(deltaTime);
		brokenBrickSpriteSL->renderBroken(deltaTime);
		brokenBrickSpriteSR->renderBroken(deltaTime);
	}
}

//  Only callable when the brick in ENTIRE
void Brick::setPosition(const glm::vec2& pos)
{
	posBrick = pos;
	entireBrickSprite->setPosition(glm::vec2(float(tileMapDispl.x + posBrick.x), float(tileMapDispl.y + posBrick.y)));
}

glm::vec2 Brick::getPosition() {
	return posBrick;
}

float Brick::getFacingDirection() {
	return facingDirection;
}

