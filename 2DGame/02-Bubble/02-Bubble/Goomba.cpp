#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Goomba.h"


// <- and -> Physics Values from ORIGINAL GAME
#define NORMAL_WALK_SPEED 1.f
#define NORMAL_FALL_SPEED -2.5f

//For reading Sprite
#define SPRITE_OFFSET_X (1.f / 3.f)
#define SPRITE_OFFSET_Y (1.f / 2.f)


enum GoombaAnims
{
	WALKING, DYING
};

enum GoombaDirection
{
	FACING_LEFT, FACING_RIGHT
};



void Goomba::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	//define physics values
	facingDirection = -1.f;
	horitzontal_speed = NORMAL_WALK_SPEED;
	vertical_speed = NORMAL_FALL_SPEED;
	is_dead = false;
	is_collidable = true;
	frames_until_death = -1;
	kill_frames = -1;

	//INIT SPRITES
	entitySpritesheet.loadFromFile("images/goomba.png", TEXTURE_PIXEL_FORMAT_RGBA);
	entitySpritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(SPRITE_OFFSET_X, SPRITE_OFFSET_Y), &entitySpritesheet, &shaderProgram);

	sprite->setNumberAnimations(2);

	sprite->setAnimationSpeed(WALKING, 8);
	sprite->addKeyframe(WALKING, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(WALKING, glm::vec2(SPRITE_OFFSET_X * 1.f, 0.f));

	sprite->setAnimationSpeed(DYING, 8);
	sprite->addKeyframe(DYING, glm::vec2(SPRITE_OFFSET_X * 2.f, 0.f));


	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(16, 16);
	sprite->changeAnimation(WALKING);
	sprite->changeDirection(FACING_LEFT);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEntity.x), float(tileMapDispl.y + posEntity.y)));
}

void Goomba::update(int deltaTime)
{
	if (frames_until_death > 0) --frames_until_death;
	else if (frames_until_death == 0) is_dead = true;
	else Entity::update(deltaTime);
}

void Goomba::takeDamage()
{
	is_collidable = false;
	sprite->changeAnimation(DYING);
	frames_until_death = 45;
}

string Goomba::whoAmI()
{

	return "KOOPA";
}
