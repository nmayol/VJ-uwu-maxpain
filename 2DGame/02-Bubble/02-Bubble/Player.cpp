#include <cmath>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


// <- and -> Physics Values from ORIGINAL GAME
#define MIN_WALK_SPEED 0.07421875f
#define MAX_WALK_SPEED 1.5625f
#define WALK_ACCELERATION 0.037109375f
#define MAX_RUN_SPEED 2.5625f
#define RUN_ACCELERATION 0.0556640625f
#define DECELERATION 0.05078125f
#define SKID_DECELERATION 0.1015625f
#define SKID_TURNAROUND_SPEED 0.5625f

// Mid-Air Momentum Physics
#define MIN_AIR_MOMENTUM_THRESHOLD 1.5625f
#define MAX_AIR_MOMENTUM_THRESHOLD 1.8125f
#define SLOW_AIR_MOMENTUM 0.037109375f
#define NORMAL_AIR_MOMENTUM 0.05078125f
#define FAST_AIR_MOMENTUM 0.0556640625f


// Jumping Physics Vales from ORIGINAL GAME
#define MIN_XSPEED_NORM_JUMP 1.f
#define MIN_XSPEED_FAST_JUMP 2.3125f
#define INITIAL_JUMP_YSPEED 4.f
#define INITIAL_FAST_JUMP_YSPEED 5.f
#define SLOW_HOLDING_GRAVITY 0.125f
#define NORMAL_HOLDING_GRAVITY 0.1171875f
#define FAST_HOLDING_GRAVITY 0.15625f
#define SLOW_GRAVITY 0.4375f
#define NORMAL_GRAVITY 0.375f
#define FAST_GRAVITY 0.5625f
#define MAX_FALL_SPEED -4.53515625f

//For reading Sprite
#define SPRITE_OFFSET_X (1.f / 16.f)
#define BIG_SPRITE_OFFSET_X (1.f / 32.f)
#define BIG_SPRITE_OFFSET_Y (1.f / 16.f)
#define SPRITE_OFFSET_Y (1.f / 16.f)

enum playerAnims
{
	STANDING, RUNNING, JUMPING, SKIDDING, CROUCHING, NONE
};


enum PlayerDirection
{
	FACING_LEFT, FACING_RIGHT
};

enum PlayerTransformation
{
	SMALL, NORMAL, FIRE
};


void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	bJumping = false;
	JumpedAndReleased = true;
	pressedPCount = 0;
	pressedPandReleased = true;
	framesUntilSlowdown = 0;
	facingDirection = 1.f;
	actual_speed = 0.f;
	vertical_speed = 2.5f;
	actualAnimation = STANDING;
	actualForm = SMALL;

	//INIT SPRITES
	smallMarioSpritesheet.loadFromFile("images/small-mario.png", TEXTURE_PIXEL_FORMAT_RGBA);
	smallMarioSprite = initSmallMarioSprite(&smallMarioSpritesheet, &shaderProgram);

	normalMarioSpritesheet.loadFromFile("images/normal-mario.png", TEXTURE_PIXEL_FORMAT_RGBA);
	normalMarioSprite = initNormalMarioSprite(0.f, &normalMarioSpritesheet, &shaderProgram);

	normalMarioSpritesheet.loadFromFile("images/normal-mario.png", TEXTURE_PIXEL_FORMAT_RGBA);
	fireMarioSprite = initNormalMarioSprite(1.f, &normalMarioSpritesheet, &shaderProgram);

	sprite = smallMarioSprite;

	tileMapDispl = tileMapPos;
	collision_box_size = glm::ivec2(16, 16);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

}

//INIT SMALL MARIO SPRITE_SHEET
Sprite* Player::initSmallMarioSprite(Texture* spritesheet, ShaderProgram* shaderProgram) {

	Sprite* newSprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(SPRITE_OFFSET_X, SPRITE_OFFSET_Y), spritesheet, shaderProgram);

	newSprite->setNumberAnimations(6);

	newSprite->setAnimationSpeed(STANDING, 8);
	newSprite->addKeyframe(STANDING, glm::vec2(SPRITE_OFFSET_X * 6.f, 0.f));

	newSprite->setAnimationSpeed(RUNNING, 8);
	newSprite->addKeyframe(RUNNING, glm::vec2(0.f, 0.f));
	newSprite->addKeyframe(RUNNING, glm::vec2(SPRITE_OFFSET_X * 1.f, 0.f));
	newSprite->addKeyframe(RUNNING, glm::vec2(SPRITE_OFFSET_X * 2.f, 0.f));

	newSprite->setAnimationSpeed(JUMPING, 8);
	newSprite->addKeyframe(JUMPING, glm::vec2(SPRITE_OFFSET_X * 4.f, 0.f));

	newSprite->setAnimationSpeed(SKIDDING, 8);
	newSprite->addKeyframe(SKIDDING, glm::vec2(SPRITE_OFFSET_X * 3.f, 0.f));

	newSprite->setAnimationSpeed(NONE, 8);
	newSprite->addKeyframe(NONE, glm::vec2(SPRITE_OFFSET_X * 14.f, 0.f));

	return newSprite;
}


//INIT NORMAL MARIO SPRITE_SHEET
Sprite* Player::initNormalMarioSprite(float baseSpriteRow, Texture* spritesheet, ShaderProgram* shaderProgram) {

	Sprite* newSprite = Sprite::createSprite(glm::ivec2(16, 32), glm::vec2(BIG_SPRITE_OFFSET_X, SPRITE_OFFSET_Y), spritesheet, shaderProgram);
	newSprite->setNumberAnimations(7);
	baseSpriteRow *= BIG_SPRITE_OFFSET_Y;

	newSprite->setAnimationSpeed(STANDING, 8);
	newSprite->addKeyframe(STANDING, glm::vec2(BIG_SPRITE_OFFSET_X * 6.f, baseSpriteRow));

	newSprite->setAnimationSpeed(RUNNING, 8);
	newSprite->addKeyframe(RUNNING, glm::vec2(0.f, baseSpriteRow));
	newSprite->addKeyframe(RUNNING, glm::vec2(BIG_SPRITE_OFFSET_X * 1.f, baseSpriteRow));
	newSprite->addKeyframe(RUNNING, glm::vec2(BIG_SPRITE_OFFSET_X * 2.f, baseSpriteRow));

	newSprite->setAnimationSpeed(JUMPING, 8);
	newSprite->addKeyframe(JUMPING, glm::vec2(BIG_SPRITE_OFFSET_X * 4.f, baseSpriteRow));

	newSprite->setAnimationSpeed(SKIDDING, 8);
	newSprite->addKeyframe(SKIDDING, glm::vec2(BIG_SPRITE_OFFSET_X * 3.f, baseSpriteRow));

	newSprite->setAnimationSpeed(CROUCHING, 8);
	newSprite->addKeyframe(CROUCHING, glm::vec2(BIG_SPRITE_OFFSET_X * 5.f, baseSpriteRow));

	newSprite->setAnimationSpeed(NONE, 0);
	newSprite->addKeyframe(NONE, glm::vec2(BIG_SPRITE_OFFSET_X * 17.f, baseSpriteRow));

	return newSprite;
}


//CHANGE MARIO FORM
void Player::setMarioForm(int formId) {

	if (actualForm != SMALL && formId == SMALL) posPlayer.y += 16.f;
	else if (actualForm == SMALL && formId != SMALL) posPlayer.y -= 16.f;

	actualForm = formId;

	switch (formId) {
	case SMALL:
		sprite = smallMarioSprite;
		collision_box_size = glm::ivec2(16, 16);
		break;

	case NORMAL:
		sprite = normalMarioSprite;
		collision_box_size = glm::ivec2(16, 32);
		break;

	default:
		sprite = fireMarioSprite;
		collision_box_size = glm::ivec2(16, 32);
		break;
	}

	if (facingDirection == -1.f) sprite->changeDirection(FACING_LEFT);
	else sprite->changeDirection(FACING_RIGHT);
	pressedPCount = 30;
	pressedPandReleased = false;
}

void Player::update(int deltaTime, bool gameCompleted, bool couldBeGoingUnderworld, bool wantsToGoOverworld)
{
	if (gameCompleted) { // Movement to the castle
		if (posPlayer.x <= 204.f * 16) {
			actualAnimation = RUNNING;
			posPlayer += glm::vec2(1.f, 0);
			if (posPlayer.y <=  16 * 16 && !map->collisionMoveDown(posPlayer, collision_box_size, &posPlayer.y))
				posPlayer.y += 8.f;
			else bJumping = false;
		}
		else actualAnimation = NONE;
	}
	else if ((actualAnimation == CROUCHING) && couldBeGoingUnderworld) {
		if (posPlayer.y >= 10.15f * 16) { // Movement into a tube OVERWORLD
			posPlayer.y = 19.f * 16;
			posPlayer.x = 49.f * 16;
			actualAnimation = RUNNING;
		}
		else posPlayer.y += 2.f;
	}
	else if (wantsToGoOverworld) { // Movement into a tube UNDERWORLD
		if (posPlayer.x >= 61 * 16) {
			posPlayer.x = 163.5 * 16;
			posPlayer.y = 8.f * 16;
			actualAnimation = JUMPING;
		}	
		else posPlayer.x += 2.f;
	}

	else {
		//load parameters early for better eficiency
		bool facingLeft = (facingDirection == -1.f);
		bool leftKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_LEFT);
		bool rightKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_RIGHT);
		bool runKeyPressed = Game::instance().getKey('z') || Game::instance().getKey('Z');
		bool downKeyPressed = Game::instance().getSpecialKey(GLUT_KEY_DOWN);
		bool upKeyPressed = false;
		if (Game::instance().getSpecialKey(GLUT_KEY_UP) || Game::instance().getKey('x') || Game::instance().getKey('X')) upKeyPressed = JumpedAndReleased;
		else JumpedAndReleased = true;

		//DEV CONTROLS
		if ((Game::instance().getKey('p') || Game::instance().getKey('P'))) {
			if (pressedPandReleased || pressedPCount == 0) setMarioForm((actualForm + 1) % 3);
		}
		else pressedPandReleased = true;
		pressedPCount--;

		//Save inputs and prevents bugs by only allowing left or right (not both)
		if (leftKeyPressed && rightKeyPressed) {
			leftKeyPressed = (facingDirection == -1.f);
			rightKeyPressed = !leftKeyPressed;
		}


		// MARIO IS MID-JUMPING
		if (bJumping) {

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
			else if (MIN_XSPEED_NORM_JUMP <= initial_jump_xspeed && initial_jump_xspeed < MIN_XSPEED_FAST_JUMP)
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
				if (actual_speed > MIN_AIR_MOMENTUM_THRESHOLD) actual_speed = std::max(-max_xspeed_allowed_jumping, actual_speed -= FAST_AIR_MOMENTUM);
				else if (initial_jump_xspeed < MAX_AIR_MOMENTUM_THRESHOLD) actual_speed = std::max(-max_xspeed_allowed_jumping, actual_speed -= SLOW_AIR_MOMENTUM);
				else actual_speed = std::max(-max_xspeed_allowed_jumping, actual_speed -= NORMAL_AIR_MOMENTUM);
			}

			posPlayer.y -= vertical_speed;
			// Check if already on floor (stop falling)
			if (map->collisionMoveUp(posPlayer, collision_box_size, &posPlayer.y)) vertical_speed *= -0.25f;
			else if (map->collisionMoveDown(posPlayer, collision_box_size, &posPlayer.y)) {
				bJumping = false;
				JumpedAndReleased = false;
				vertical_speed = std::max(-1.5f, vertical_speed);

				//Moving backwards, apply special logic
				if (actual_speed < 0) {
					actual_speed *= -1.f;
					actualAnimation = SKIDDING;

				}
			}
			//APPLY MOVEMENT
			posPlayer.x += facingDirection * actual_speed;
			if ((facingLeft && map->collisionMoveLeft(posPlayer, collision_box_size, &posPlayer.x)) || (!facingLeft && map->collisionMoveRight(posPlayer, collision_box_size, &posPlayer.x))) actual_speed = 0;

		}

		//MARIO IS NOT JUMPING
		else {

			// MARIO WANTS TO JUMP
			if (upKeyPressed) {
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
				if (runKeyPressed) max_xspeed_allowed_jumping = MAX_RUN_SPEED;
				else max_xspeed_allowed_jumping = MAX_WALK_SPEED;

				posPlayer.y -= vertical_speed;
			}
			else {

				//MARIO IS TURNING AROUND. If SKIDDING is necesary, apply that ANIMATION
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


				//APPLY CROUCHING
				if (!leftKeyPressed && !rightKeyPressed && downKeyPressed && actualForm != SMALL) {
					if (actualAnimation == SKIDDING) {
						facingDirection *= -1.f;
						facingLeft = !facingLeft;
					}
					framesUntilSlowdown = 0;
					actual_speed = std::max(0.f, actual_speed - DECELERATION);
					posPlayer.x += actual_speed * facingDirection;
					if ((facingLeft && map->collisionMoveLeft(posPlayer, collision_box_size, &posPlayer.x)) || (!facingLeft && map->collisionMoveRight(posPlayer, collision_box_size, &posPlayer.x))) actual_speed = 0;
					actualAnimation = CROUCHING;
				}

				//APPLY SKIDDING ANIMATION and move speed reduction
				else if (actualAnimation == SKIDDING) {
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
					else {
						posPlayer.x += facingDirection * actual_speed * -1.f;
						if ((!facingLeft && map->collisionMoveLeft(posPlayer, collision_box_size, &posPlayer.x)) || (facingLeft && map->collisionMoveRight(posPlayer, collision_box_size, &posPlayer.x))) {
							actualAnimation = STANDING;
							actual_speed = 0;
						}
					}
				}

				// APPLY RUN/WALK Movement LEFT or RIGHT
				else if (leftKeyPressed || rightKeyPressed) {
					if (leftKeyPressed) {
						facingDirection = -1.f;
						sprite->changeDirection(FACING_LEFT);
					}
					else {
						facingDirection = 1.f;
						sprite->changeDirection(FACING_RIGHT);
					}


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
					if ((facingLeft && map->collisionMoveLeft(posPlayer, collision_box_size, &posPlayer.x)) || (!facingLeft && map->collisionMoveRight(posPlayer, collision_box_size, &posPlayer.x))) {
						actualAnimation = STANDING;
						actual_speed = 0;
					}

				}


				// NOT pressing any key -> Deacelerating until STANDING
				else if (actual_speed > 0) {
					actualAnimation = RUNNING;
					//IF at MAX SPEED, wait 10 frames then deacelerate
					if (framesUntilSlowdown > 0) {
						framesUntilSlowdown -= 1;
						posPlayer.x += actual_speed * facingDirection;
						if ((facingLeft && map->collisionMoveLeft(posPlayer, collision_box_size, &posPlayer.x)) || (!facingLeft && map->collisionMoveRight(posPlayer, collision_box_size, &posPlayer.x))) {
							actualAnimation = STANDING;
							actual_speed = 0;
							framesUntilSlowdown = 0;
						}
					}
					else {
						actual_speed = std::max(0.f, actual_speed - DECELERATION);
						if (actual_speed == 0.f) actualAnimation = STANDING;
						else {
							posPlayer.x += actual_speed * facingDirection;
							if ((facingLeft && map->collisionMoveLeft(posPlayer, collision_box_size, &posPlayer.x)) || (!facingLeft && map->collisionMoveRight(posPlayer, collision_box_size, &posPlayer.x))) {
								actualAnimation = STANDING;
								actual_speed = 0;
								framesUntilSlowdown = 0;
							}
						}

					}
				}
				else actualAnimation = STANDING;

				// APPLY FALLING PHYSICS
				posPlayer.y -= vertical_speed;
				if (!map->collisionMoveDown(posPlayer, collision_box_size, &posPlayer.y))
				{
					sprite->changeAnimation(JUMPING);
					bJumping = true;
					if (actualAnimation == SKIDDING) {
						if (facingLeft) sprite->changeDirection(FACING_RIGHT);
						else sprite->changeDirection(FACING_LEFT);
						facingDirection *= -1.f;
						facingLeft = !facingLeft;
					}
					if (runKeyPressed) max_xspeed_allowed_jumping = MAX_RUN_SPEED;
					else max_xspeed_allowed_jumping = MAX_WALK_SPEED;
				}
			}
		}
	}
	int anim = sprite->animation();
	if (sprite->animation() != actualAnimation && !bJumping)
		sprite->changeAnimation(actualAnimation);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	sprite->update(deltaTime);

}

void Player::render()
{
	sprite->render();
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2& pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

glm::vec2 Player::getPosition() {
	return posPlayer;
}

glm::vec2 Player::getPositioninTM() {
	return posPlayer + glm::vec2(float(tileMapDispl.x), float(tileMapDispl.y));
}

float Player::getFacingDirection() {
	return facingDirection;
}


