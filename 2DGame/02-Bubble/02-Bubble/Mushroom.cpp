#include "Mushroom.h"


#define NORMAL_WALK_SPEED 1.5f
#define NORMAL_FALL_SPEED -2.5f
#define GRAVITY 0.375f;

//For reading Sprite
#define SPRITE_OFFSET_X (1.f / 9.f)

enum mushroomDirection
{
	FACING_LEFT, FACING_RIGHT
};

void Mushroom::init(const glm::ivec2& tileMapPos, const glm::vec2& initial_pos, TileMap* tileMap, ShaderProgram& shaderProgram)
{
	//define physics values
	facingDirection = 1.f;
	horitzontal_speed = NORMAL_WALK_SPEED;
	gravity = GRAVITY;
	vertical_speed = base_vertical_speed =  NORMAL_FALL_SPEED;
	is_dead = false;
	is_collidable = false;
	kill_frames = -1;
	posEntity = initial_pos;
	map = tileMap;

	//INIT SPRITES
	entitySpritesheet.loadFromFile("images/items.png", TEXTURE_PIXEL_FORMAT_RGBA);
	entitySpritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(SPRITE_OFFSET_X, 1.f), &entitySpritesheet, &shaderProgram);

	sprite->setNumberAnimations(1);
	sprite->setAnimationSpeed(0, 8);
	sprite->addKeyframe(0, glm::vec2(0.f, 0.f));


	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(16, 16);
	sprite->changeAnimation(0);
	sprite->changeDirection(FACING_RIGHT);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEntity.x), float(tileMapDispl.y + posEntity.y)));
}

bool Mushroom::takeDamage()
{
    return false;
}

string Mushroom::whoAmI()
{
    return "MUSHROOM";
}
