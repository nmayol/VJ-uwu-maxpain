#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4

// <- and -> Physics Values from ORIGINAL GAME
#define MIN_WALK_SPEED 0.07421875f * 2
#define MAX_WALK_SPEED 1.5625f * 2
#define WALK_ACCELERATION 0.037109375f * 2
#define MAX_RUN_SPEED 2.5625f * 2
#define RUN_ACCELERATION 0.0556640625f * 2
#define DECELERATION 0.05078125f * 2
#define SKID_DECELERATION 0.1015625f * 2
#define SKID_TURNAROUND_SPEED 0.5625f * 2

// Mid-Air Momentum Physics
#define MIN_AIR_MOMENTUM_THRESHOLD 1.5625f * 2
#define MAX_AIR_MOMENTUM_THRESHOLD 1.8125f * 2
#define SLOW_AIR_MOMENTUM 0.037109375f * 2
#define NORMAL_AIR_MOMENTUM 0.05078125f * 2
#define FAST_AIR_MOMENTUM 0.0556640625f * 2


// Jumping Physics Vales from ORIGINAL GAME
#define MIN_XSPEED_NORM_JUMP 1.f * 2
#define MIN_XSPEED_FAST_JUMP 2.3125f * 2
#define INITIAL_JUMP_YSPEED 4.f * 2
#define INITIAL_FAST_JUMP_YSPEED 5. * 2
#define SLOW_HOLDING_GRAVITY 0.125f * 2
#define NORMAL_HOLDING_GRAVITY 0.1171875f * 2
#define FAST_HOLDING_GRAVITY 0.15625f * 2
#define SLOW_GRAVITY 0.4375f * 2
#define NORMAL_GRAVITY 0.375f * 2
#define FAST_GRAVITY 0.15625f * 2
#define MAX_FALL_SPEED -4.53515625f * 2

#define SPRITE_OFFSET (1.f / 14.f)

enum PlayerAnims
{
	STANDING, RUNNING, JUMPING, SKIDDING, DYING
};

enum PlayerDirection
{
	FACING_LEFT, FACING_RIGHT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{	
	bJumping = false;
	framesUntilSlowdown = 0;
	facingDirection = 1.f;
	actual_speed = 0.f;
	spritesheet.loadFromFile("images/mario-small.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(SPRITE_OFFSET, 1.f), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(5);
	
		sprite->setAnimationSpeed(STANDING, 8);
		sprite->addKeyframe(STANDING, glm::vec2(SPRITE_OFFSET*6.f, 0.f));
		
		sprite->setAnimationSpeed(RUNNING, 8);
		sprite->addKeyframe(RUNNING, glm::vec2(0.f, 0.f));
		sprite->addKeyframe(RUNNING, glm::vec2(SPRITE_OFFSET * 1.f, 0.f));
		sprite->addKeyframe(RUNNING, glm::vec2(SPRITE_OFFSET * 2.f, 0.f));
		
		sprite->setAnimationSpeed(JUMPING, 8);
		sprite->addKeyframe(JUMPING, glm::vec2(SPRITE_OFFSET * 4.f, 0.f));

		sprite->setAnimationSpeed(SKIDDING, 8);
		sprite->addKeyframe(SKIDDING, glm::vec2(SPRITE_OFFSET * 3.f, 0.f));
		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(30, 32);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);

	//load parameters early for better eficiency
	int actualAnimation = sprite->animation();
	bool facingLeft = (facingDirection == -1.f);
	bool leftKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_LEFT);
	bool rightKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_RIGHT);
	bool upKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_UP);

	//Save inputs and prevents bugs by only allowing left or right (not both)
	if (leftKeyPressed && rightKeyPressed) {
		leftKeyPressed = (facingDirection == -1.f);
		rightKeyPressed = !leftKeyPressed;
	}
	

	//Detect turn around and apply. If SKIDDING is necesary, apply that ANIMATION
	if ((leftKeyPressed && !facingLeft) || (rightKeyPressed && facingLeft)) {
		if (facingLeft) sprite->changeDirection(FACING_RIGHT);
		else sprite->changeDirection(FACING_LEFT);
		facingDirection *= -1.f;
		facingLeft = !facingLeft;

		// if player is already skidding and wants to re-change direction || Player not skidd decides to change direction -> skidd || changes direction but not enouch speed to skid
		if (actualAnimation == SKIDDING) actualAnimation = RUNNING;
		else if (actual_speed >= SKID_TURNAROUND_SPEED) actualAnimation = SKIDDING;
		else actual_speed = MIN_WALK_SPEED;

	}


	//APPLY SKIDDING ANIMATION and move speed reduction
	if (actualAnimation == SKIDDING) {
		actual_speed -= SKID_DECELERATION;

		// if under threshold for turning around while SKIDDING, turn around
		if (actual_speed <= SKID_TURNAROUND_SPEED && ((leftKeyPressed && facingLeft) || (rightKeyPressed && !facingLeft))) {
			actualAnimation = RUNNING;
		}
		// SLOWED DOWN until STOPPING
		else if (actual_speed <= 0) {
			actualAnimation = STANDING;
			actual_speed = 0.f;
		}
		else posPlayer.x += facingDirection * actual_speed * -1.f;
	}


	// APPLY RUN/WALK Movement LEFT or RIGHT
	if(actualAnimation != SKIDDING && (leftKeyPressed || rightKeyPressed)) {

		// if NOT RUNNING, START RUNNING
		if (actualAnimation != RUNNING) {
			actualAnimation = RUNNING;
			actual_speed = MIN_WALK_SPEED;
		}

		// Pressing RUN Key, Mario is Running
		if (Game::instance().getKey('z') || Game::instance().getKey('Z')) {
			if (actual_speed < MAX_RUN_SPEED) actual_speed = std::min(MAX_RUN_SPEED, actual_speed + RUN_ACCELERATION);

			//At MAX_SPEED, WAIT 10 Frames before slowing down
			if (actual_speed == MAX_RUN_SPEED) framesUntilSlowdown = 10;
		}
		// NOT Pressing RUN Key, Mario is Walking
		else if (actual_speed < MAX_WALK_SPEED) actual_speed = std::min(MAX_WALK_SPEED, actual_speed + WALK_ACCELERATION);

		//update position acording to speed and direction facing
		posPlayer.x += facingDirection * actual_speed;

		if(map->collisionMoveLeft(posPlayer, collision_box_size))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STANDING);
		}
	}
	// NOT pressing any key -> Deacelerating until STANDING
	else if (actualAnimation == RUNNING) {
		//IF at MAX SPEED, wait 10 frames then deacelerate
		if (framesUntilSlowdown > 0) {
			framesUntilSlowdown -= 1;
			posPlayer.x += actual_speed * facingDirection;
		}
		else {
			actual_speed = std::max(0.f, actual_speed - DECELERATION);
			if (actual_speed == 0.f) actualAnimation = STANDING;
			else posPlayer.x += actual_speed * facingDirection;
		}
	}

	// STARTED JUMPING THIS FRAME
	if (!bJumping && upKeyPressed) {

		//if initial XSPEED was SLOW or FAST -> Jump has different behaviours
		if (actual_speed < MIN_XSPEED_FAST_JUMP) vertical_speed = INITIAL_JUMP_YSPEED;
		else vertical_speed = INITIAL_FAST_JUMP_YSPEED;

		initial_jump_xspeed = actual_speed;
		sprite->changeAnimation(JUMPING);
		bJumping = true;
		if (actualAnimation == SKIDDING) {
			if (facingLeft) sprite->changeDirection(FACING_RIGHT);
			else sprite->changeDirection(FACING_LEFT);
			facingDirection *= -1.f;
			facingLeft = !facingLeft;
		}
		if (Game::instance().getKey('z') || Game::instance().getKey('Z')) max_xspeed_allowed_jumping = MAX_RUN_SPEED;
		else max_xspeed_allowed_jumping = MAX_WALK_SPEED;

		posPlayer.y -= vertical_speed;
	}

	// MID JUMP
	else if(bJumping)
	{

		// VERTICAL MOVEMENT
		if (vertical_speed < MAX_FALL_SPEED) vertical_speed = MAX_FALL_SPEED;
		else if (initial_jump_xspeed < MIN_XSPEED_NORM_JUMP) 
		{
			if (upKeyPressed) vertical_speed -= SLOW_HOLDING_GRAVITY;
			else vertical_speed -= SLOW_GRAVITY;
		}
		else if (initial_jump_xspeed >= MIN_XSPEED_FAST_JUMP) 
		{
			if (upKeyPressed) vertical_speed -= FAST_HOLDING_GRAVITY;
			else vertical_speed -= FAST_GRAVITY;
		}
		else //if (MIN_XSPEED_NORM_JUMP <= initial_jump_xspeed && initial_jump_xspeed < MIN_XSPEED_FAST_JUMP) {
		{
			if (upKeyPressed) vertical_speed -= NORMAL_HOLDING_GRAVITY;
			else vertical_speed -= NORMAL_GRAVITY;
		}

		// HORIZONTAL MOVEMENT
		if (leftKeyPressed && facingLeft || rightKeyPressed && !facingLeft) 
		{
			//Moving Forward
			if (actual_speed < MIN_AIR_MOMENTUM_THRESHOLD) actual_speed = std::min(max_xspeed_allowed_jumping, actual_speed += SLOW_AIR_MOMENTUM);
			else actual_speed = std::min(max_xspeed_allowed_jumping, actual_speed += FAST_AIR_MOMENTUM);
			actualAnimation = RUNNING;
		}
		else if (leftKeyPressed && !facingLeft || rightKeyPressed && facingLeft)
		{
			//Moving Backward
			if (actual_speed > MIN_AIR_MOMENTUM_THRESHOLD ) actual_speed = std::min(max_xspeed_allowed_jumping, actual_speed -= FAST_AIR_MOMENTUM);
			else if (initial_jump_xspeed < MAX_AIR_MOMENTUM_THRESHOLD) actual_speed  = std::min(max_xspeed_allowed_jumping, actual_speed -= SLOW_AIR_MOMENTUM);
			else actual_speed = std::min(max_xspeed_allowed_jumping, actual_speed -= NORMAL_AIR_MOMENTUM);
		}

		posPlayer.y -= vertical_speed;
	}      
	else
	{
		posPlayer.y += FALL_STEP;
		if(map->collisionMoveDown(posPlayer, collision_box_size, &posPlayer.y))
		{
			if(Game::instance().getSpecialKey(GLUT_KEY_UP))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
	}
	
	if (sprite->animation() != actualAnimation && !bJumping) sprite->changeAnimation(actualAnimation);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




