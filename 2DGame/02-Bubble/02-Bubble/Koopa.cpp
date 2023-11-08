#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Koopa.h"


// <- and -> Physics Values from ORIGINAL GAME
#define NORMAL_WALK_SPEED 1.f
#define SHELL_LAUNCH_SPEED 3.f
#define NORMAL_FALL_SPEED -2.5f

//For reading Sprite
#define SPRITE_OFFSET_X (1.f / 4.f)
#define SPRITE_OFFSET_Y (1.f / 3.f)


enum KoopaAnims
{
	WALKING, SHELL, DESHELLING
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
	frames_until_respawn = -1;
	kill_frames = -1;
	shell_in_movement = false;
	is_dead = false;
	is_collidable = true;

	//INIT SPRITES
	entitySpritesheet.loadFromFile("images/koopa.png", TEXTURE_PIXEL_FORMAT_RGBA);
	entitySpritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(glm::ivec2(16, 27), glm::vec2(SPRITE_OFFSET_X, SPRITE_OFFSET_Y), &entitySpritesheet, &shaderProgram);

	sprite->setNumberAnimations(3);

	sprite->setAnimationSpeed(WALKING, 8);
	sprite->addKeyframe(WALKING, glm::vec2(SPRITE_OFFSET_X * 1.f, 0.f));
	sprite->addKeyframe(WALKING, glm::vec2(0.f, 0.f));

	sprite->setAnimationSpeed(DESHELLING, 8);
	sprite->addKeyframe(DESHELLING, glm::vec2(SPRITE_OFFSET_X * 2.f, 0.f));
	sprite->addKeyframe(DESHELLING, glm::vec2(SPRITE_OFFSET_X * 3.f, 0.f));

	sprite->setAnimationSpeed(SHELL, 8);
	sprite->addKeyframe(SHELL, glm::vec2(SPRITE_OFFSET_X * 3.f, 0.f));

	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(16, 27);
	sprite->changeAnimation(WALKING);
	sprite->changeDirection(FACING_LEFT);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEntity.x), float(tileMapDispl.y + posEntity.y)));
}

void Koopa::update(int deltaTime)
{
	if (frames_until_respawn >= 0 && kill_frames == -1) {
		if (frames_until_respawn == 120) sprite->changeAnimation(DESHELLING);
		else if (frames_until_respawn == 0) {
			sprite->changeAnimation(WALKING);
			horitzontal_speed = NORMAL_WALK_SPEED;
		}
		frames_until_respawn--;
		sprite->update(deltaTime);
	}
	else Entity::update(deltaTime);
}

int Koopa::detectPlayerCollision(glm::vec2 posPlayer, bool Falling, const glm::ivec2& size)
{
	if (horitzontal_speed == NORMAL_WALK_SPEED) return Entity::detectPlayerCollision(posPlayer, Falling, size);
	else {
		// When in shellmode, size is diferent
		float x0, x1, y0, entx1;
		y0 = (posPlayer.y + size.y);
		x0 = posPlayer.x;
		x1 = x0 + size.x;
		entx1 = posEntity.x + collision_box_size.x;

		if (((x0 > posEntity.x && x0 < entx1) || (x1 > posEntity.x && x1 < entx1)) && y0 >= 11.f + posEntity.y && y0 <= posEntity.y + collision_box_size.y) {
			if (horitzontal_speed == 0) return 3;
			else if (y0 > (19.f + posEntity.y)) return 1;
			else return 2;
		}
		return 0;
	}
}

bool Koopa::takeDamage()
{
	//Base koopa has base speed, shell has 0 and moving shell has shell launch speed
	if (horitzontal_speed == 0) {
		frames_until_respawn = -1;
		shell_in_movement = true;
		sprite->changeAnimation(SHELL);
		horitzontal_speed = SHELL_LAUNCH_SPEED;
	}
	else {
		horitzontal_speed = 0;
		frames_until_respawn = 8 * 60;
		sprite->changeAnimation(SHELL);
	}
	return false;
}

bool Koopa::canKillEnemies()
{
	return horitzontal_speed == SHELL_LAUNCH_SPEED;
}

string Koopa::whoAmI()
{

	return "KOOPA";
}
