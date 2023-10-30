#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Brick.h"
#include "Game.h"


// <- and -> Physics Values from ORIGINAL GAME
#define MIN_WALK_SPEED 0.07421875f
#define MAX_WALK_SPEED 1.5625f
#define WALK_ACCELERATION 0.037109375f
#define MAX_RUN_SPEED 2.5625f
#define RUN_ACCELERATION 0.0556640625f
#define DECELERATION 0.05078125f
#define SKID_DECELERATION 0.1015625f
#define SKID_TURNAROUND_SPEED 0.5625f

// Mid-Air Momentum Physics
#define MIN_AIR_MOMENTUM_THRESHOLD 1.5625f
#define MAX_AIR_MOMENTUM_THRESHOLD 1.8125f
#define SLOW_AIR_MOMENTUM 0.037109375f
#define NORMAL_AIR_MOMENTUM 0.05078125f
#define FAST_AIR_MOMENTUM 0.0556640625f


// Jumping Physics Vales from ORIGINAL GAME
#define MIN_XSPEED_NORM_JUMP 1.f
#define MIN_XSPEED_FAST_JUMP 2.3125f
#define INITIAL_YSPEED 20.f

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
	actual_speed = 0.f;
	
	actualAnimation = ENTIRE;
	actualForm = ENTIRE;
	

	//INIT SPRITES
	brickSpritesheet.loadFromFile("images/Brick.png",TEXTURE_PIXEL_FORMAT_RGBA);
	entireBrickSprite = initEntireBrickSprite(&brickSpritesheet, &shaderProgram);
	brokenBrickSpriteSL = initBrokenBrickSprite(&brickSpritesheet, &shaderProgram, 0, 0);
	brokenBrickSpriteSR = initBrokenBrickSprite(&brickSpritesheet, &shaderProgram, 0, 1);
	brokenBrickSpriteIR = initBrokenBrickSprite(&brickSpritesheet, &shaderProgram, 1, 1);
	brokenBrickSpriteIL = initBrokenBrickSprite(&brickSpritesheet, &shaderProgram, 1, 0);
	brokenBrickSpriteIL->changeDirection(0);
	brokenBrickSpriteSL->changeDirection(0);
	

	sprite = entireBrickSprite;
	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(16, 16);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBrick.x), float(tileMapDispl.y + posBrick.y)));
	sprite->changeAnimation(actualAnimation);
	

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
	// newSprite->addKeyframe(ENTIRE, glm::vec2(23.f * SPRITE_OFFSET_X, 0.f));
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
		posBrokenBrickIL = posBrick + glm::vec2(0, 8);
		posBrokenBrickIR = posBrick + glm::vec2(8, 8);
		posBrokenBrickSL = posBrick + glm::vec2(0, 0);
		posBrokenBrickSR = posBrick + glm::vec2(8, 0);
		vys = -INITIAL_YSPEED * sin(3.14159265f / 2.2f);
		vxs = INITIAL_YSPEED * cos(3.14159265f / 2.2f);
		vxi = INITIAL_YSPEED * cos(3.14159265f / 2.5f);
		vyi = -INITIAL_YSPEED * sin(3.14159265f / 2.5f);

	}
	if (frames_from_breaking >= 0) {

		brokenBrickSpriteSL->update(deltaTime);
		brokenBrickSpriteSR->update(deltaTime);
		brokenBrickSpriteIR->update(deltaTime);
		brokenBrickSpriteIL->update(deltaTime);

		float t = deltaTime / 100.f;
		
		posBrokenBrickSR = posBrokenBrickSR + glm::vec2(vxs * t, vys * t - 5 * t * t);
		posBrokenBrickSL = posBrokenBrickSL + glm::vec2(-vxs * t, vys * t - 5 * t * t);
		posBrokenBrickIR = posBrokenBrickIR + glm::vec2(vxi * t, vyi * t - 5 * t * t);
		posBrokenBrickIL = posBrokenBrickIL + glm::vec2(-vxi * t, vyi*t - 5*t*t);
		
			
		
		vys += 10 * t;
		vyi += 10 * t;
		

	

		brokenBrickSpriteSL->setPosition(posBrokenBrickSL);
		brokenBrickSpriteSR->setPosition(posBrokenBrickSR);
		brokenBrickSpriteIR->setPosition(posBrokenBrickIR);
		brokenBrickSpriteIL->setPosition(posBrokenBrickIL);

		

	
		++frames_from_breaking;
	}
	else {
		sprite->update(deltaTime);
		if (sprite->animation() != actualAnimation) sprite->changeAnimation(actualAnimation);
		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBrick.x), float(tileMapDispl.y + posBrick.y)));

	}

}



void Brick::render(int deltaTime)
{
	if (actualAnimation == ENTIRE) {
		sprite->render();
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
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBrick.x), float(tileMapDispl.y + posBrick.y)));
}

glm::vec2 Brick::getPosition() {
	return posBrick;
}

glm::vec2 Brick::getPositioninTM() {
	return posBrick + glm::vec2(float(tileMapDispl.x), float(tileMapDispl.y));
}

float Brick::getFacingDirection() {
	return facingDirection;
}

