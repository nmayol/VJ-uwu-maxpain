#pragma once
#include "Sprite.h"

class FloatingScore
{

public:
	FloatingScore(const int& scoreValue, const glm::vec2& pos, ShaderProgram& shaderProgram);
	FloatingScore::~FloatingScore();

	bool update(int deltaTime);
	void render();

private:

	int framesShowed;
	float vertical_speed;
	glm::vec2 pos;

	Texture floatingScoreSpritesheet;
	Sprite* sprite;
};

