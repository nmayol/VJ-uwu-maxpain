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


private:

	Sprite* backgroundSprite;


	Texture background;
	Texture Leters;
	Texture coinAnimation;
};

