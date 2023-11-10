#include "FloatingScore.h"

#define Y_SPRITE_OFFSET (1.f / 6.f)

FloatingScore::FloatingScore(const int& scoreValue, const glm::vec2& pos, ShaderProgram& shaderProgram)
{
	framesShowed = 30;
	vertical_speed = 1.5f;

	floatingScoreSpritesheet.loadFromFile("images/floatingScores.png", TEXTURE_PIXEL_FORMAT_RGBA);
	floatingScoreSpritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(glm::ivec2(16, 8), glm::vec2(0.5, Y_SPRITE_OFFSET), &floatingScoreSpritesheet, &shaderProgram);
	sprite->setNumberAnimations(1);
	sprite->setAnimationSpeed(0, 8);

	score_value = scoreValue;

	//set certain Sprite
	switch (score_value)
	{
	case 100:
		sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
		break;

	case 200:
		sprite->addKeyframe(0, glm::vec2(0.f, Y_SPRITE_OFFSET * 1.f));
		break;

	case 400:
		sprite->addKeyframe(0, glm::vec2(0.f, Y_SPRITE_OFFSET * 2.f));
		break;

	case 500:
		sprite->addKeyframe(0, glm::vec2(0.f, Y_SPRITE_OFFSET * 3.f));
		break;

	case 800:
		sprite->addKeyframe(0, glm::vec2(0.f, Y_SPRITE_OFFSET * 4.f));
		break;

	case 1000:
		sprite->addKeyframe(0, glm::vec2(0.5f, 0.f));
		break;

	case 2000:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 1.f));
		break;

	case 4000:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 2.f));
		break;

	case 5000:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 3.f));
		break;

	case 8000:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 4.f));
		break;

	default:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 5.f));
		break;
	}

	sprite->changeAnimation(0);
	sprite->setPosition(pos);
	this->pos = pos;
}

FloatingScore::FloatingScore(const int& scoreValue, const glm::vec2& pos, ShaderProgram& shaderProgram, int combo)
{
	framesShowed = 30;
	vertical_speed = 1.5f;

	floatingScoreSpritesheet.loadFromFile("images/floatingScores.png", TEXTURE_PIXEL_FORMAT_RGBA);
	floatingScoreSpritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(glm::ivec2(16, 8), glm::vec2(0.5, Y_SPRITE_OFFSET), &floatingScoreSpritesheet, &shaderProgram);
	sprite->setNumberAnimations(1);
	sprite->setAnimationSpeed(0, 8);

	score_value = scoreValue;
	while (combo != 0) {
		if (score_value == 100 || score_value == 400) 
			score_value += 100;
		else if (score_value == 200 || score_value == 800)
			score_value += 200;
		else if (score_value == 500)
			score_value += 300;
		else if (score_value == 1000 || score_value == 4000)
			score_value += 1000;
		else if (score_value == 2000 || score_value == 8000)
			score_value += 2000;
		else if (score_value == 5000)
			score_value += 3000;
		combo = combo - 1;
	}

	//set certain Sprite
	switch (score_value)
	{
	case 100:
		sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
		break;

	case 200:
		sprite->addKeyframe(0, glm::vec2(0.f, Y_SPRITE_OFFSET * 1.f));
		break;

	case 400:
		sprite->addKeyframe(0, glm::vec2(0.f, Y_SPRITE_OFFSET * 2.f));
		break;

	case 500:
		sprite->addKeyframe(0, glm::vec2(0.f, Y_SPRITE_OFFSET * 3.f));
		break;

	case 800:
		sprite->addKeyframe(0, glm::vec2(0.f, Y_SPRITE_OFFSET * 4.f));
		break;

	case 1000:
		sprite->addKeyframe(0, glm::vec2(0.5f, 0.f));
		break;

	case 2000:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 1.f));
		break;

	case 4000:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 2.f));
		break;

	case 5000:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 3.f));
		break;

	case 8000:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 4.f));
		break;

	default:
		sprite->addKeyframe(0, glm::vec2(0.5f, Y_SPRITE_OFFSET * 5.f));
		break;
	}

	sprite->changeAnimation(0);
	sprite->setPosition(pos);
	this->pos = pos;
}

FloatingScore::~FloatingScore() {
	if (sprite != NULL)
		delete sprite;
}

bool FloatingScore::update(int deltaTime)
{
	pos.y -= vertical_speed;
	sprite->setPosition(pos);
	framesShowed--;
	return framesShowed == 0;
}

void FloatingScore::render()
{
	sprite->render();
}

int FloatingScore::getValueAfterCombo()
{
	return score_value;
}