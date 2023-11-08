#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Sprite.h"
#include "PlayerInterface.h"

class MainScreen
{

public:
	MainScreen();
	~MainScreen();

	void init();
	void update(int deltaTime);
	void render();
	void setEndingScreenTo(const int& endingLastPlaythrough, const int& finalScore, const int& finalCoins, const int& finalLevel);
	bool goToGame();

private:
	void initShaders();

private:
	int actual_screen_showed;
	int delay_Frames;
	int selected_Option;
	bool pressed_and_released;
	bool go_to_game;

	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	Texture cursorAndCoinSpritesheet;
	Sprite* cursorSymbol;
	Sprite* coinSymbol;

	Texture mainScreenBackground;
	Sprite* mainScreen;

	Texture secondaryScreensBackground;
	Sprite* instructionsScreen;
	Sprite* creditsScreen;
	Sprite* gameOverScreen;
	Sprite* gameCompletedScreen;

	PlayerInterface* player_iface;
};

