#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Koopa.h"


// <- and -> Physics Values from ORIGINAL GAME
#define NORMAL_WALK_SPEED 1.f
#define NORMAL_FALL_SPEED -2.5f

//For reading Sprite
#define SPRITE_OFFSET_X (1.f / 2.f)
#define SPRITE_OFFSET_Y (1.f / 3.f)


enum KoopaAnims
{
	WALKING
};

enum KoopaDirection
{
	FACING_LEFT, FACING_RIGHT
};



void Koopa::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	//define physics values
	facingDirection = -1.f;
	horitzontal_speed = NORMAL_WALK_SPEED;
	vertical_speed = NORMAL_FALL_SPEED;
	is_dead = false;
	is_collidable = true;

	//INIT SPRITES
	entitySpritesheet.loadFromFile("images/koopa.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(16, 27), glm::vec2(SPRITE_OFFSET_X, SPRITE_OFFSET_Y), &entitySpritesheet, &shaderProgram);

	sprite->setNumberAnimations(2);

	sprite->setAnimationSpeed(WALKING, 8);
	sprite->addKeyframe(WALKING, glm::vec2(SPRITE_OFFSET_X * 1.f, 0.f));
	sprite->addKeyframe(WALKING, glm::vec2(0.f, 0.f));


	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(16, 27);
	sprite->changeAnimation(WALKING);
	sprite->changeDirection(FACING_LEFT);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEntity.x), float(tileMapDispl.y + posEntity.y)));
}

void Koopa::update(int deltaTime)
{
	Entity::update(deltaTime);
}

void Koopa::takeDamage()
{
	is_collidable = false;
	is_dead = true;
}

string Koopa::whoAmI()
{

	return "I AM KOOPA!";
}
