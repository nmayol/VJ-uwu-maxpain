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

// This method must be implemented by deriving classes
// void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram) {}

void Entity::update(int deltaTime)
{
	sprite->update(deltaTime);
		
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

float Entity::getFacingDirection() {
	return facingDirection;
}

bool Entity::isEntityDead()
{
	return is_dead;
}