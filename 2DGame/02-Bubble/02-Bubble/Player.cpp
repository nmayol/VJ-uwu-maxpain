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

// Physics Values
#define MIN_WALK_SPEED 0.07421875f * 2
#define MAX_WALK_SPEED 1.5625f * 2
#define WALK_ACCELERATION 0.037109375f * 2
#define MAX_RUN_SPEED 2.5625f * 2
#define RUN_ACCELERATION 0.0556640625f * 2
#define DECELERATION 0.05078125f * 2
#define SKID_DECELERATION 0.1015625f * 2
#define SKID_TURNAROUND_SPEED 0.5625f * 2

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
	facingDirection = -1.f;
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
	
	bool leftKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_LEFT);
	bool rightKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_RIGHT);

	

	//Detect turn around and apply. If SKIDDING is necesary, apply that ANIMATION
	if ((leftKeyPressed && facingDirection != -1.f) || (rightKeyPressed && facingDirection != 1.f)) {
		if (facingDirection == 1.f) sprite->changeDirection(FACING_LEFT);
		else sprite->changeDirection(FACING_RIGHT);
		facingDirection *= -1.f;

		if (actual_speed > SKID_TURNAROUND_SPEED) sprite->changeAnimation(SKIDDING);
		else actual_speed = MIN_WALK_SPEED;
	}

	//APPLY SKIDDING ANIMATION and move speed reduction
	if (sprite->animation() == SKIDDING) {
		actual_speed -= SKID_DECELERATION;

		// if under threshold for turning around while SKIDDING, turn around
		if (actual_speed <= SKID_TURNAROUND_SPEED && ((leftKeyPressed && facingDirection == -1.f) || (rightKeyPressed && facingDirection == 1.f))) {
			sprite->changeAnimation(RUNNING);
		}
		// SLOWED DOWN until STOPPING
		else if (actual_speed <= 0) {
			sprite->changeAnimation(STANDING);
			actual_speed = 0.f;
		}
		else posPlayer.x += facingDirection * actual_speed * -1.f;
	}

	// APPLY RUN/WALK Movement LEFT or RIGHT
	else if((leftKeyPressed || rightKeyPressed)) {

		// if NOT RUNNING, START RUNNING
		if (sprite->animation() != RUNNING) {
			sprite->changeAnimation(RUNNING);
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
	else {	
		// NOT pressing any key -> Deacelerating until STANDING
		if (sprite->animation() == RUNNING)

			//IF at MAX SPEED, wait 10 frames then deacelerate
			if (framesUntilSlowdown > 0) {
				framesUntilSlowdown -= 1;
				posPlayer.x += actual_speed * facingDirection;
			}
			else {
				actual_speed = std::max(0.f, actual_speed - DECELERATION);
				if (actual_speed == 0.f) sprite->changeAnimation(STANDING);
				else posPlayer.x += actual_speed * facingDirection;
			}
	}
	
	if(bJumping)
	{
		if (sprite->animation() != JUMPING)
			sprite->changeAnimation(JUMPING);

		jumpAngle += JUMP_ANGLE_STEP;
		if(jumpAngle == 180)
		{
			bJumping = false;
			sprite->changeAnimation(STANDING);
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
			if(jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, collision_box_size, &posPlayer.y);
		}
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




