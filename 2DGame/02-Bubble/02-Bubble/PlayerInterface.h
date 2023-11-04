#pragma once
#include "Sprite.h"

class PlayerInterface
{
public:
	PlayerInterface();
	~PlayerInterface();

	void init(ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();
	void setScreenX(const float& new_x);


private:
	glm::vec2 posEntity;
	glm::ivec2 tileMapDispl;
	Sprite* backgroundSprite;
	Texture background;
};
