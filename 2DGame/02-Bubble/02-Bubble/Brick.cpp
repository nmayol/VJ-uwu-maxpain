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


enum BrickDirection
{
	FACING_LEFT, FACING_RIGHT
};

//enum BrickTransformation
//{
//	ENTIRE, BROKEN
//};


void Brick::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	
	pressedPCount = 0;
	pressedPandReleased = true;
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

	//normalBrickSpritesheet.loadFromFile("images/normal-Brick.png", TEXTURE_PIXEL_FORMAT_RGBA);
	//fireBrickSprite = initNormalBrickSprite(1.f, &normalBrickSpritesheet, &shaderProgram);
	//sprite = smallBrickSprite;

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

	//if (facingDirection == -1.f) sprite->changeDirection(FACING_LEFT);
	//else sprite->changeDirection(FACING_RIGHT);
	pressedPCount = 30;
	pressedPandReleased = false;
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


	//DEV CONTROLS
	if ((Game::instance().getKey('p') || Game::instance().getKey('P'))) {
		if (pressedPandReleased || pressedPCount == 0) setBrickForm((actualForm + 1) % 2);
	}
	else pressedPandReleased = true;
	pressedPCount--;

	//Save inputs and prevents bugs by only allowing left or right (not both)
	if (leftKeyPressed && rightKeyPressed) {
		leftKeyPressed = (facingDirection == -1.f);
		rightKeyPressed = !leftKeyPressed;
	}


	// Brick IS MID-JUMPING
	//if (bJumping) {

	//	// VERTICAL MOVEMENT
	//	if (vertical_speed < MAX_FALL_SPEED) vertical_speed = MAX_FALL_SPEED;
	//	else if (initial_jump_xspeed < MIN_XSPEED_NORM_JUMP)
	//	{
	//		if (upKeyPressed) vertical_speed -= SLOW_HOLDING_GRAVITY;
	//		else vertical_speed -= SLOW_GRAVITY;
	//	}
	//	else if (initial_jump_xspeed >= MIN_XSPEED_FAST_JUMP)
	//	{
	//		if (upKeyPressed) vertical_speed -= FAST_HOLDING_GRAVITY;
	//		else vertical_speed -= FAST_GRAVITY;
	//	}
	//	else if (MIN_XSPEED_NORM_JUMP <= initial_jump_xspeed && initial_jump_xspeed < MIN_XSPEED_FAST_JUMP)
	//	{
	//		if (upKeyPressed) vertical_speed -= NORMAL_HOLDING_GRAVITY;
	//		else vertical_speed -= NORMAL_GRAVITY;
	//	}

	//	// HORIZONTAL MOVEMENT
	//	if (leftKeyPressed && facingLeft || rightKeyPressed && !facingLeft)
	//	{
	//		//Moving Forward
	//		if (actual_speed < MIN_AIR_MOMENTUM_THRESHOLD) actual_speed = std::min(max_xspeed_allowed_jumping, actual_speed += SLOW_AIR_MOMENTUM);
	//		else actual_speed = std::min(max_xspeed_allowed_jumping, actual_speed += FAST_AIR_MOMENTUM);
	//		actualAnimation = RUNNING;
	//	}
	//	else if (leftKeyPressed && !facingLeft || rightKeyPressed && facingLeft)
	//	{
	//		//Moving Backward
	//		if (actual_speed > MIN_AIR_MOMENTUM_THRESHOLD) actual_speed = std::max(-max_xspeed_allowed_jumping, actual_speed -= FAST_AIR_MOMENTUM);
	//		else if (initial_jump_xspeed < MAX_AIR_MOMENTUM_THRESHOLD) actual_speed = std::max(-max_xspeed_allowed_jumping, actual_speed -= SLOW_AIR_MOMENTUM);
	//		else actual_speed = std::max(-max_xspeed_allowed_jumping, actual_speed -= NORMAL_AIR_MOMENTUM);
	//	}

	//	posBrick.y -= vertical_speed;
	//	// Check if already on floor (stop falling)
	//	if (map->collisionMoveUp(posBrick, collision_box_size, &posBrick.y)) vertical_speed *= -0.25f;
	//	else if (map->collisionMoveDown(posBrick, collision_box_size, &posBrick.y)) {
	//		bJumping = false;
	//		JumpedAndReleased = false;
	//		vertical_speed = std::max(-1.5f, vertical_speed);

	//		//Moving backwards, apply special logic
	//		if (actual_speed < 0) {
	//			actual_speed *= -1.f;
	//			actualAnimation = SKIDDING;

	//		}
	//	}
	//	//APPLY MOVEMENT
	//	posBrick.x += facingDirection * actual_speed;
	//	if ((facingLeft && map->collisionMoveLeft(posBrick, collision_box_size, &posBrick.x)) || (!facingLeft && map->collisionMoveRight(posBrick, collision_box_size, &posBrick.x))) actual_speed = 0;

	//}

	//Brick IS NOT JUMPING
	//else {

	//	// Brick WANTS TO JUMP
	//	if (upKeyPressed) {
	//		//if initial XSPEED was SLOW or FAST -> Jump has different behaviours
	//		if (actual_speed < MIN_XSPEED_FAST_JUMP) vertical_speed = INITIAL_JUMP_YSPEED;
	//		else vertical_speed = INITIAL_FAST_JUMP_YSPEED;

	//		initial_jump_xspeed = actual_speed;
	//		sprite->changeAnimation(JUMPING);
	//		bJumping = true;
	//		if (actualAnimation == SKIDDING) {
	//			if (facingLeft) sprite->changeDirection(FACING_RIGHT);
	//			else sprite->changeDirection(FACING_LEFT);
	//			facingDirection *= -1.f;
	//			facingLeft = !facingLeft;
	//		}
	//		if (runKeyPressed) max_xspeed_allowed_jumping = MAX_RUN_SPEED;
	//		else max_xspeed_allowed_jumping = MAX_WALK_SPEED;

	//		posBrick.y -= vertical_speed;
	//	}
	//	else {

	//		//Brick IS TURNING AROUND. If SKIDDING is necesary, apply that ANIMATION
	//		if ((leftKeyPressed && !facingLeft) || (rightKeyPressed && facingLeft)) {
	//			if (facingLeft) sprite->changeDirection(FACING_RIGHT);
	//			else sprite->changeDirection(FACING_LEFT);
	//			facingDirection *= -1.f;
	//			facingLeft = !facingLeft;

	//			// if Brick is already skidding and wants to re-change direction || Brick not skidd decides to change direction -> skidd || changes direction but not enouch speed to skid
	//			if (actualAnimation == SKIDDING) actualAnimation = RUNNING;
	//			else if (actual_speed >= SKID_TURNAROUND_SPEED) actualAnimation = SKIDDING;
	//			else actual_speed = MIN_WALK_SPEED;
	//		}


	//		//APPLY CROUCHING
	//		if (!leftKeyPressed && !rightKeyPressed && downKeyPressed && actualForm != SMALL) {
	//			if (actualAnimation == SKIDDING) {
	//				facingDirection *= -1.f;
	//				facingLeft = !facingLeft;
	//			}
	//			framesUntilSlowdown = 0;
	//			actual_speed = std::max(0.f, actual_speed - DECELERATION);
	//			posBrick.x += actual_speed * facingDirection;
	//			if ((facingLeft && map->collisionMoveLeft(posBrick, collision_box_size, &posBrick.x)) || (!facingLeft && map->collisionMoveRight(posBrick, collision_box_size, &posBrick.x))) actual_speed = 0;
	//			actualAnimation = CROUCHING;
	//		}

	//		//APPLY SKIDDING ANIMATION and move speed reduction
	//		else if (actualAnimation == SKIDDING) {
	//			actual_speed -= SKID_DECELERATION;

	//			// if under threshold for turning around while SKIDDING, turn around
	//			if (actual_speed <= SKID_TURNAROUND_SPEED && ((leftKeyPressed && facingLeft) || (rightKeyPressed && !facingLeft))) {
	//				actualAnimation = RUNNING;
	//			}
	//			// SLOWED DOWN until STOPPING
	//			else if (actual_speed <= 0) {
	//				actualAnimation = STANDING;
	//				actual_speed = 0.f;
	//			}
	//			else {
	//				posBrick.x += facingDirection * actual_speed * -1.f;
	//				if ((!facingLeft && map->collisionMoveLeft(posBrick, collision_box_size, &posBrick.x)) || (facingLeft && map->collisionMoveRight(posBrick, collision_box_size, &posBrick.x))) {
	//					actualAnimation = STANDING;
	//					actual_speed = 0;
	//				}
	//			}
	//		}

	//		// APPLY RUN/WALK Movement LEFT or RIGHT
	//		else if (leftKeyPressed || rightKeyPressed) {
	//			if (leftKeyPressed) {
	//				facingDirection = -1.f;
	//				sprite->changeDirection(FACING_LEFT);
	//			}
	//			else {
	//				facingDirection = 1.f;
	//				sprite->changeDirection(FACING_RIGHT);
	//			}


	//			// if NOT RUNNING, START RUNNING
	//			if (actualAnimation != RUNNING) {
	//				actualAnimation = RUNNING;
	//				actual_speed = MIN_WALK_SPEED;
	//			}

	//			// Pressing RUN Key, Brick is Running
	//			if (Game::instance().getKey('z') || Game::instance().getKey('Z')) {
	//				if (actual_speed < MAX_RUN_SPEED) actual_speed = std::min(MAX_RUN_SPEED, actual_speed + RUN_ACCELERATION);

	//				//At MAX_SPEED, WAIT 10 Frames before slowing down
	//				if (actual_speed == MAX_RUN_SPEED) framesUntilSlowdown = 10;
	//			}
	//			// NOT Pressing RUN Key, Brick is Walking
	//			else if (actual_speed < MAX_WALK_SPEED) actual_speed = std::min(MAX_WALK_SPEED, actual_speed + WALK_ACCELERATION);

	//			//update position acording to speed and direction facing
	//			posBrick.x += facingDirection * actual_speed;
	//			if ((facingLeft && map->collisionMoveLeft(posBrick, collision_box_size, &posBrick.x)) || (!facingLeft && map->collisionMoveRight(posBrick, collision_box_size, &posBrick.x))) {
	//				actualAnimation = STANDING;
	//				actual_speed = 0;
	//			}

	//		}


	//		// NOT pressing any key -> Deacelerating until STANDING
	//		else if (actual_speed > 0) {
	//			actualAnimation = RUNNING;
	//			//IF at MAX SPEED, wait 10 frames then deacelerate
	//			if (framesUntilSlowdown > 0) {
	//				framesUntilSlowdown -= 1;
	//				posBrick.x += actual_speed * facingDirection;
	//				if ((facingLeft && map->collisionMoveLeft(posBrick, collision_box_size, &posBrick.x)) || (!facingLeft && map->collisionMoveRight(posBrick, collision_box_size, &posBrick.x))) {
	//					actualAnimation = STANDING;
	//					actual_speed = 0;
	//					framesUntilSlowdown = 0;
	//				}
	//			}
	//			else {
	//				actual_speed = std::max(0.f, actual_speed - DECELERATION);
	//				if (actual_speed == 0.f) actualAnimation = STANDING;
	//				else {
	//					posBrick.x += actual_speed * facingDirection;
	//					if ((facingLeft && map->collisionMoveLeft(posBrick, collision_box_size, &posBrick.x)) || (!facingLeft && map->collisionMoveRight(posBrick, collision_box_size, &posBrick.x))) {
	//						actualAnimation = STANDING;
	//						actual_speed = 0;
	//						framesUntilSlowdown = 0;
	//					}
	//				}

	//			}
	//		}
	//		else actualAnimation = STANDING;

	//		// APPLY FALLING PHYSICS
	//		posBrick.y -= vertical_speed;
	//		if (!map->collisionMoveDown(posBrick, collision_box_size, &posBrick.y))
	//		{
	//			sprite->changeAnimation(JUMPING);
	//			bJumping = true;
	//			if (actualAnimation == SKIDDING) {
	//				if (facingLeft) sprite->changeDirection(FACING_RIGHT);
	//				else sprite->changeDirection(FACING_LEFT);
	//				facingDirection *= -1.f;
	//				facingLeft = !facingLeft;
	//			}
	//			if (runKeyPressed) max_xspeed_allowed_jumping = MAX_RUN_SPEED;
	//			else max_xspeed_allowed_jumping = MAX_WALK_SPEED;
	//		}
	//	}
	//}

	if (sprite->animation() != actualAnimation) sprite->changeAnimation(actualAnimation);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBrick.x), float(tileMapDispl.y + posBrick.y)));
}

void Brick::render()
{
	sprite->render();
}

void Brick::setTileMap(TileMap* tileMap)
{
	map = tileMap;
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


