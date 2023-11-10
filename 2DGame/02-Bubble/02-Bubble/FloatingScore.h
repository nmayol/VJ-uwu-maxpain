#pragma once
#include "Sprite.h"

class FloatingScore
{

public:
	FloatingScore(const int& scoreValue, const glm::vec2& pos, ShaderProgram& shaderProgram);
	FloatingScore(const int& scoreValue, const glm::vec2& pos, ShaderProgram& shaderProgram, int combo);
	FloatingScore::~FloatingScore();

	bool update(int deltaTime);
	void render();

	int getValueAfterCombo();

private:

	int framesShowed;
	int score_value;
	float vertical_speed;
	glm::vec2 pos;

	Texture floatingScoreSpritesheet;
	Sprite* sprite;
};

