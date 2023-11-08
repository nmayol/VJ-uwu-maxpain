#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GL/glut.h>
#include "MainScreen.h"
#include "Game.h"

#define Y_SPRITE_OFFSET (1.f / 3.f)
#define X_SPRITE_OFFSET (1.f / 16.f)

MainScreen::MainScreen()
{
	player_iface = NULL;
}

enum showingScreen {
	MAIN_SCREEN, INSTRUCTIONS_SCREEN, CREDITS_SCREEN, GAME_OVER_SCREEN, GAME_COMPLETED_SCREEN
};

MainScreen::~MainScreen()
{
	if (coinSymbol != NULL)
		delete coinSymbol;
	if (mainScreen != NULL)
		delete mainScreen;
	if (cursorSymbol != NULL)
		delete cursorSymbol;
	if (instructionsScreen != NULL)
		delete instructionsScreen;
	if (creditsScreen != NULL)
		delete creditsScreen;
	if (player_iface != NULL)
		delete player_iface;
}

void MainScreen::init()
{
	initShaders();
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT + 40), 41.f);
	currentTime = 0.f;
	delay_Frames = 0;
	selected_Option = 0;
	go_to_game = false;
	pressed_and_released = true;
	actual_screen_showed = MAIN_SCREEN;

	//MAIN SCREEN BACKGROUND
	mainScreenBackground.loadFromFile("images/screens/mainScreen.png", TEXTURE_PIXEL_FORMAT_RGBA);
	mainScreenBackground.setMagFilter(GL_NEAREST);
	mainScreen = Sprite::createSprite(glm::ivec2(256, 240), glm::vec2(1.f, 1.f), &mainScreenBackground, &texProgram);
	mainScreen->setNumberAnimations(1);
	mainScreen->setAnimationSpeed(0, 8);
	mainScreen->addKeyframe(0, glm::vec2(0.f, 0.f));
	mainScreen->changeAnimation(0);
	mainScreen->setPosition(glm::vec2(0.f, 41.f));

	//INSTRUCTIONS SCREEN BACKGROUND
	secondaryScreensBackground.loadFromFile("images/screens/secondaryScreens.png", TEXTURE_PIXEL_FORMAT_RGBA);
	secondaryScreensBackground.setMagFilter(GL_NEAREST);
	instructionsScreen = Sprite::createSprite(glm::ivec2(256, 240), glm::vec2(0.5f, 0.5f), &secondaryScreensBackground, &texProgram);
	instructionsScreen->setNumberAnimations(1);
	instructionsScreen->setAnimationSpeed(0, 8);
	instructionsScreen->addKeyframe(0, glm::vec2(0.f, 0.f));
	instructionsScreen->changeAnimation(0);
	instructionsScreen->setPosition(glm::vec2(0.f, 41.f));

	//CREDITS SCREEN BACKGROUND
	creditsScreen = Sprite::createSprite(glm::ivec2(256, 240), glm::vec2(0.5f, 0.5f), &secondaryScreensBackground, &texProgram);
	creditsScreen->setNumberAnimations(1);
	creditsScreen->setAnimationSpeed(0, 8);
	creditsScreen->addKeyframe(0, glm::vec2(0.5f, 0.f));
	creditsScreen->changeAnimation(0);
	creditsScreen->setPosition(glm::vec2(0.f, 41.f));

	//GAME_OVER SCREEN BACKGROUND
	gameOverScreen = Sprite::createSprite(glm::ivec2(256, 240), glm::vec2(0.5f, 0.5f), &secondaryScreensBackground, &texProgram);
	gameOverScreen->setNumberAnimations(1);
	gameOverScreen->setAnimationSpeed(0, 8);
	gameOverScreen->addKeyframe(0, glm::vec2(0.f, 0.5f));
	gameOverScreen->changeAnimation(0);
	gameOverScreen->setPosition(glm::vec2(0.f, 41.f));

	//GAME_COMPLETED SCREEN BACKGROUND
	gameCompletedScreen = Sprite::createSprite(glm::ivec2(256, 240), glm::vec2(0.5f, 0.5f), &secondaryScreensBackground, &texProgram);
	gameCompletedScreen->setNumberAnimations(1);
	gameCompletedScreen->setAnimationSpeed(0, 8);
	gameCompletedScreen->addKeyframe(0, glm::vec2(0.5f, 0.5f));
	gameCompletedScreen->changeAnimation(0);
	gameCompletedScreen->setPosition(glm::vec2(0.f, 41.f));



	//COIN SYMBOL
	cursorAndCoinSpritesheet.loadFromFile("images/screens/text.png", TEXTURE_PIXEL_FORMAT_RGBA);
	cursorAndCoinSpritesheet.setMagFilter(GL_NEAREST);
	coinSymbol = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(X_SPRITE_OFFSET, Y_SPRITE_OFFSET), &cursorAndCoinSpritesheet, &texProgram);
	coinSymbol->setNumberAnimations(1);
	coinSymbol->setAnimationSpeed(0, 8);
	coinSymbol->addKeyframe(0, glm::vec2(X_SPRITE_OFFSET * 9.f, Y_SPRITE_OFFSET * 2.f));
	coinSymbol->addKeyframe(0, glm::vec2(X_SPRITE_OFFSET * 9.f, Y_SPRITE_OFFSET * 2.f));
	coinSymbol->addKeyframe(0, glm::vec2(X_SPRITE_OFFSET * 9.f, Y_SPRITE_OFFSET * 2.f));
	coinSymbol->addKeyframe(0, glm::vec2(X_SPRITE_OFFSET * 9.f, Y_SPRITE_OFFSET * 2.f));
	coinSymbol->addKeyframe(0, glm::vec2(X_SPRITE_OFFSET * 9.f, Y_SPRITE_OFFSET * 2.f));
	coinSymbol->addKeyframe(0, glm::vec2(X_SPRITE_OFFSET * 10.f, Y_SPRITE_OFFSET * 2.f));
	coinSymbol->addKeyframe(0, glm::vec2(X_SPRITE_OFFSET * 11.f, Y_SPRITE_OFFSET * 2.f));
	coinSymbol->addKeyframe(0, glm::vec2(X_SPRITE_OFFSET * 10.f, Y_SPRITE_OFFSET * 2.f));
	coinSymbol->changeAnimation(0);
	coinSymbol->setPosition(glm::vec2(88.f, 65.f));

	//CURSOR SYMBOL
	cursorSymbol = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(X_SPRITE_OFFSET, Y_SPRITE_OFFSET), &cursorAndCoinSpritesheet, &texProgram);
	cursorSymbol->setNumberAnimations(1);
	cursorSymbol->setAnimationSpeed(0, 8);
	cursorSymbol->addKeyframe(0, glm::vec2(X_SPRITE_OFFSET * 12.f, Y_SPRITE_OFFSET * 2.f));
	cursorSymbol->changeAnimation(0);
	cursorSymbol->setPosition(glm::vec2(72.f, 185.f));

	//STATS FOR ENDING SCREEN
	player_iface = new PlayerInterface();
	player_iface->init(texProgram);
	player_iface->setScreenXandY(0.f, 0.f);
	
}

void MainScreen::update(int deltaTime)
{
	currentTime += deltaTime;
	coinSymbol->update(deltaTime);

	if (actual_screen_showed == GAME_COMPLETED || actual_screen_showed == GAME_OVER) player_iface->render();

	//Cursor Moving
	if (actual_screen_showed == MAIN_SCREEN) {
		if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) {
			if (pressed_and_released || delay_Frames == 0) {
				selected_Option = (selected_Option + 1) % 3;
				cursorSymbol->setPosition(glm::vec2(72.f, 185.f + selected_Option * 16.f));
				delay_Frames = 30;
				pressed_and_released = false;
			}
		}
		else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
			if (pressed_and_released || delay_Frames == 0) {
				selected_Option = (3 + selected_Option - 1) % 3;
				cursorSymbol->setPosition(glm::vec2(72.f, 185.f + selected_Option * 16.f));
				delay_Frames = 30;
				pressed_and_released = false;
			}
		}
		else if (Game::instance().getKey(13) || Game::instance().getKey(' ')) {
			if (pressed_and_released) {
				if (selected_Option == 0) {
					// start game
					go_to_game = true;
				}
				else {
					//GO TO CREDITS OR INSTRUCTIONS SCREEN
					actual_screen_showed = selected_Option;
					pressed_and_released = false;
				}
			}
		}
		else pressed_and_released = true;
		if (delay_Frames != 0) delay_Frames--;
	}

	// GO TO MAIN SCREEN
	else if (Game::instance().getKey(13) || Game::instance().getKey(' ')) {
		if (pressed_and_released) {
			actual_screen_showed = MAIN_SCREEN;
			pressed_and_released = false;
		}
	}
	else pressed_and_released = true;
}

void MainScreen::render()
{
	glm::mat4 modelview = glm::mat4(1.0f);
	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	switch (actual_screen_showed)
	{
	case INSTRUCTIONS_SCREEN:
		instructionsScreen->render();
		break;

	case CREDITS_SCREEN:
		creditsScreen->render();
		break;

	case GAME_OVER_SCREEN:
		gameOverScreen->render();
		player_iface->render();
		break;

	case GAME_COMPLETED_SCREEN:
		gameCompletedScreen->render();
		player_iface->render();
		break;

	default:
		mainScreen->render();
		coinSymbol->render();
		cursorSymbol->render();
		break;
	}
}

void MainScreen::setEndingScreenTo(const int& endingLastPlaythrough, const int& finalScore, const int& finalCoins, const int& finalLevel)
{
	// 1 -> GAME_OVER
	// 2 -> GAME_COMPLETED
	if (endingLastPlaythrough == GAME_OVER) actual_screen_showed = 3;
	else if (endingLastPlaythrough == GAME_COMPLETED) actual_screen_showed = 4;
	go_to_game = false;

	player_iface->reset();
	player_iface->changeActualLevel(finalLevel);
	player_iface->addCoins(finalCoins);
	player_iface->addToScore(finalScore);
	
}

void MainScreen::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if (!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

bool MainScreen::goToGame() {
	return go_to_game;
}