#pragma once
#include"Sprite.h"

class loadingScreen
{
public:
	loadingScreen();
	~loadingScreen();

	void init(ShaderProgram& shaderProgram, const int& level_number, const int& number_lives);
	void update_level(const int& level_number, const int& number_lives);
	void render();

private:
	Texture background;
	Sprite* backgroundSprite;

	Texture levelSymbolSpritesheet;
	Sprite* levelSymbol;
	Sprite* livesSymbol;
};

