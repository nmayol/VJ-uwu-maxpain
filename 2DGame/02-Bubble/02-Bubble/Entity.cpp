#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Entity.h"


enum EntityDirection
{
	FACING_LEFT, FACING_RIGHT
};

enum PlayerColisionResult
{
	NOTHING, PLAYER_TAKES_DMG, ENTITY_TAKES_DMG
};

// This method must be implemented by deriving classes
// void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram) {}

void Entity::update(int deltaTime)
{
	sprite->update(deltaTime);
		
	if (kill_frames > 0) {
		kill_frames--;
		posEntity.x += facingDirection * horitzontal_speed;
		posEntity.y -= vertical_speed;
		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEntity.x), float(tileMapDispl.y + posEntity.y)));
		return;
	}
	if (kill_frames == 0) {
		is_dead = true;
		return;
	}
	//APPLY MOVEMENT
	posEntity.x += facingDirection * horitzontal_speed;
	bool facingLeft = (facingDirection == -1.f);
	if (facingLeft && map->collisionMoveLeft(posEntity, collision_box_size, &posEntity.x)) {
		sprite->changeDirection(FACING_RIGHT);
		facingDirection = 1.f;
	}
	else if (!facingLeft && map->collisionMoveRight(posEntity, collision_box_size, &posEntity.x)) {
		sprite->changeDirection(FACING_LEFT);
			facingDirection = -1.f;
	}

	//APPLY FALLING PHYSICS
	posEntity.y -= vertical_speed;
	map->collisionMoveDown(posEntity, collision_box_size, &posEntity.y);

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEntity.x), float(tileMapDispl.y + posEntity.y)));

}

glm::ivec2 Entity::getSize()
{
	return collision_box_size;
}

bool Entity::detectCollision(glm::vec2* posEntity, float facingDirection, const glm::ivec2& size)
{
	float x0 = posEntity->x;				// left-most side
	float x1 = posEntity->x + size.x;		// right-most side

	float y0 = this->posEntity.x;								// left-most side
	float y1 = this->posEntity.x + collision_box_size.x;		// right-most side

	if (!is_collidable) return false;
	else if (facingDirection == 1.f && x1 > y0 && x0 < y1) {
		posEntity->x = y0 - size.x - 2.f;
		return true;
	}
	else if (facingDirection != 1.f && x0 < y1 && y0 < x1){
		posEntity->x = y1 + 2.f;
		return true;
	}
	return false;
}

int Entity::detectPlayerCollision(glm::vec2 posPlayer, bool Falling, const glm::ivec2& size)
{
	float x0, x1, y0, entx1;
	y0 = (posPlayer.y + size.y);
	x0 = posPlayer.x;
	x1 = x0 + size.x;
	entx1 = posEntity.x + collision_box_size.x;

	if (((x0 > posEntity.x && x0 < entx1) || (x1 > posEntity.x && x1 < entx1)) && y0 >= posEntity.y && y0 <= posEntity.y + collision_box_size.y) {
		if (y0 > (posEntity.y + collision_box_size.y * 0.5)) return PLAYER_TAKES_DMG;
		else return ENTITY_TAKES_DMG;
	}
	return NOTHING;
}

void Entity::kill()
{
	is_collidable = false;
	vertical_speed = 2.75f;
	horitzontal_speed = 1.f;
	sprite->flipVertically();
	kill_frames = 20;

}

void Entity::render()
{
	sprite->render();
}

void Entity::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Entity::setPosition(const glm::vec2& pos)
{
	posEntity = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEntity.x), float(tileMapDispl.y + posEntity.y)));
}

glm::vec2 Entity::getPosition() {
	return posEntity;
}

glm::vec2 Entity::getPositioninTM() {
	return posEntity + glm::vec2(float(tileMapDispl.x), float(tileMapDispl.y));
}

bool Entity::isCollidable()
{
	return is_collidable;
}

float Entity::getFacingDirection() {
	return facingDirection;
}

void Entity::changeFacingDirection()
{
	if (horitzontal_speed != 0) {
		if (facingDirection == -1.f) {
			sprite->changeDirection(FACING_RIGHT);
			facingDirection = 1.f;
		}
		else {
			sprite->changeDirection(FACING_LEFT);
			facingDirection = -1.f;
		}
	}
}

void Entity::changeFacingDirection(const float& new_direction)
{
	facingDirection = new_direction;
	if (new_direction == -1.f) {
		sprite->changeDirection(FACING_LEFT);
	}
	else sprite->changeDirection(FACING_RIGHT);
}

bool Entity::isEntityDead()
{
	return is_dead;
}

bool Entity::canKillEnemies()
{
	return false;
}
