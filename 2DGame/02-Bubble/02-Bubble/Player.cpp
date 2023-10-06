#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4

#define SPRITE_OFFSET (1.f / 14.f)

enum PlayerAnims
{
	STAND, RUNNING, JUMPING, STOPPING, DYING
};

enum PlayerDirection
{
	FACING_LEFT, FACING_RIGHT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	bJumping = false;
	facingLeft = true;
	spritesheet.loadFromFile("images/mario-small.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(SPRITE_OFFSET, 1.f), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(5);
	
		sprite->setAnimationSpeed(STAND, 8);
		sprite->addKeyframe(STAND, glm::vec2(SPRITE_OFFSET*6.f, 0.f));
		
		sprite->setAnimationSpeed(RUNNING, 8);
		sprite->addKeyframe(RUNNING, glm::vec2(0.f, 0.f));
		sprite->addKeyframe(RUNNING, glm::vec2(SPRITE_OFFSET * 1.f, 0.f));
		sprite->addKeyframe(RUNNING, glm::vec2(SPRITE_OFFSET * 2.f, 0.f));
		
		sprite->setAnimationSpeed(JUMPING, 8);
		sprite->addKeyframe(JUMPING, glm::vec2(SPRITE_OFFSET * 4.f, 0.f));

		sprite->setAnimationSpeed(STOPPING, 8);
		sprite->addKeyframe(STOPPING, glm::vec2(SPRITE_OFFSET * 3.f, 0.f));
		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(30, 32);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);
	if(Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		sprite->changeDirection(FACING_LEFT);
		if(sprite->animation() != RUNNING)
			sprite->changeAnimation(RUNNING);
		posPlayer.x -= 2;
		if(map->collisionMoveLeft(posPlayer, collision_box_size))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STAND);
		}
	}
	else if(Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		sprite->changeDirection(FACING_RIGHT);
		if(sprite->animation() != RUNNING)
			sprite->changeAnimation(RUNNING);
		posPlayer.x += 2;
		if(map->collisionMoveRight(posPlayer, collision_box_size))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(STAND);
		}
	}
	else
	{
		if(sprite->animation() == RUNNING)
			sprite->changeAnimation(STAND);
	}
	
	if(bJumping)
	{
		if (sprite->animation() != JUMPING)
			sprite->changeAnimation(JUMPING);

		jumpAngle += JUMP_ANGLE_STEP;
		if(jumpAngle == 180)
		{
			bJumping = false;
			sprite->changeAnimation(STAND);
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




