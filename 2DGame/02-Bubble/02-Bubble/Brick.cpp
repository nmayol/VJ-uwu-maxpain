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
#define INITIAL_JUMP_YSPEED 4.f
#define INITIAL_FAST_JUMP_YSPEED 5.f
#define SLOW_HOLDING_GRAVITY 0.125f
#define NORMAL_HOLDING_GRAVITY 0.1171875f
#define FAST_HOLDING_GRAVITY 0.15625f
#define SLOW_GRAVITY 0.4375f
#define NORMAL_GRAVITY 0.375f
#define FAST_GRAVITY 0.5625f
#define MAX_FALL_SPEED -4.53515625f

//For reading Sprite
#define SPRITE_OFFSET_X (1.f / 16.f)
#define BIG_SPRITE_OFFSET_X (1.f / 17.f)
#define SPRITE_OFFSET_Y (1.f / 2.f)

enum BrickAnims
{
	ENTIRE, BROKEN
};



void Brick::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{

	framesUntilSlowdown = 0;
	facingDirection = 1.f;
	actual_speed = 0.f;
	vertical_speed = 2.5f;
	actualAnimation = ENTIRE;
	actualForm = ENTIRE;

	//INIT SPRITES
	entireBrickSpritesheet.loadFromFile("images/Brick.png", TEXTURE_PIXEL_FORMAT_RGBA);
	entireBrickSprite = initEntireBrickSprite(&entireBrickSpritesheet, &shaderProgram);

	//normalBrickSpritesheet.loadFromFile("images/normal-Brick.png", TEXTURE_PIXEL_FORMAT_RGBA);
	//normalBrickSprite = initNormalBrickSprite(0.f, &normalBrickSpritesheet, &shaderProgram);


	sprite = entireBrickSprite;
	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(16, 16);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBrick.x), float(tileMapDispl.y + posBrick.y)));

}

//INIT SMALL Brick SPRITE_SHEET
Sprite* Brick::initEntireBrickSprite(Texture* spritesheet, ShaderProgram* shaderProgram) {

	Sprite* newSprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(SPRITE_OFFSET_X, SPRITE_OFFSET_Y), spritesheet, shaderProgram);

	newSprite->setNumberAnimations(2);

	newSprite->setAnimationSpeed(ENTIRE, 8);
	newSprite->addKeyframe(ENTIRE, glm::vec2(0.f, 0.f));

	newSprite->setAnimationSpeed(BROKEN, 8);
	newSprite->addKeyframe(BROKEN, glm::vec2(SPRITE_OFFSET_X * 5.f, 0.f));

	return newSprite;
}



//CHANGE Brick FORM
void Brick::setBrickForm(int formId) {

	if (actualForm != ENTIRE && formId == ENTIRE) posBrick.y += 8.f;

	actualForm = formId;

	switch (formId) {
	case BROKEN:
		sprite = normalBrickSprite;
		collision_box_size = glm::ivec2(16, 16);
		break;

	default:
		sprite = entireBrickSprite;
		collision_box_size = glm::ivec2(16, 16);
		break;
	}

}

void Brick::update(int deltaTime)
{
	sprite->update(deltaTime);

	//load parameters early for better eficiency
	bool facingLeft = (facingDirection == -1.f);
	bool leftKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_LEFT);
	bool rightKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_RIGHT);
	bool runKeyPressed = Game::instance().getKey('z') || Game::instance().getKey('Z');
	bool downKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_DOWN);
	bool upKeyPressed = false;




	//Save inputs and prevents bugs by only allowing left or right (not both)
	if (leftKeyPressed && rightKeyPressed) {
		leftKeyPressed = (facingDirection == -1.f);
		rightKeyPressed = !leftKeyPressed;
	}


	if (sprite->animation() != actualAnimation) sprite->changeAnimation(actualAnimation);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBrick.x), float(tileMapDispl.y + posBrick.y)));
}

void Brick::render()
{
	sprite->render();
}


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

