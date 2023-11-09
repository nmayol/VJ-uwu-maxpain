#include "Coin.h"

#define NORMAL_RISE_SPEED 4.5f
#define GRAVITY 0.215f

enum EntityDirection
{
	FACING_LEFT, FACING_RIGHT
};

void Coin::init(const glm::ivec2& tileMapPos, const glm::vec2& initial_pos, TileMap* tileMap, ShaderProgram& shaderProgram)
{
	//define physics values
	facingDirection = 1.f;
	gravity = GRAVITY;
	vertical_speed = NORMAL_RISE_SPEED;
	base_vertical_speed = -NORMAL_RISE_SPEED;
	is_dead = false;
	is_collidable = false;
	kill_frames = 38;		//spawned as a killed entity (lasts a few frames and disapears)
	posEntity = initial_pos + glm::vec2(4.f, 0.f);
	map = tileMap;

	//INIT SPRITES
	entitySpritesheet.loadFromFile("images/coin.png", TEXTURE_PIXEL_FORMAT_RGBA);
	entitySpritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(glm::ivec2(8, 16), glm::vec2(0.25f, 1.f), &entitySpritesheet, &shaderProgram);

	sprite->setNumberAnimations(1);
	sprite->setAnimationSpeed(0, 8);
	sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(0, glm::vec2(0.25f, 0.f));
	sprite->addKeyframe(0, glm::vec2(0.5f, 0.f));
	sprite->addKeyframe(0, glm::vec2(0.75f, 0.f));


	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(8, 16);
	sprite->changeAnimation(0);
	sprite->changeDirection(FACING_RIGHT);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEntity.x + 4.f), float(tileMapDispl.y + posEntity.y)));
}

void Coin::update(int timeDelta)
{
	if (kill_frames == 0) {
		is_dead = true;
		return;
	}

	kill_frames--;
	posEntity.y -= vertical_speed;
	vertical_speed = std::max(base_vertical_speed * 2.25f, vertical_speed - gravity);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEntity.x), float(tileMapDispl.y + posEntity.y)));
	sprite->update(timeDelta);
}

int Coin::detectPlayerCollision(glm::vec2 posPlayer, bool Falling, const glm::ivec2& size)
{
	return 0; //disables colision with player. Dirty but eficient (coding-wise & runtime-wise)
}

bool Coin::takeDamage()
{
	return false;
}

string Coin::whoAmI()
{
	return "MUSHROOM";
}
