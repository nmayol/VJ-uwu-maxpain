#pragma once
#include"Sprite.h"


class loadingScreen
{
public:
	loadingScreen();
	~loadingScreen();

	void init(ShaderProgram& shaderProgram, const int& level_number, const int& number_lives);
	void update(const int& level_number, const int& number_lives);
	void setTimeoutScreen();
	void setLoadingScreen();
	void render();

private:

	bool renderTimeout;

	Texture loadingAndTimeoutbackground;
	Sprite* loadingBackgroundSprite;
	Sprite* timeoutBackgroundSprite;

	Texture levelSymbolSpritesheet;
	Sprite* levelSymbol;
	Sprite* livesSymbol;
};