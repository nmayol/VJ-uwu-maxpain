#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init()
{

	bPlay = true;
	inGameScreen = false;
	glClearColor(0.390625f, 0.58203125f, 0.92578125f, 1.0f); // Blue sky color
	SoundController::instance()->init();
	mainScreen.init();
	scene.init();
}

bool Game::update(int deltaTime)
{
	if (inGameScreen) {
		scene.update(deltaTime);
		int SceneState = scene.actualGameState();

		if (SceneState == GAME_OVER || SceneState == GAME_COMPLETED)
		{
			inGameScreen = false;
			mainScreen.setEndingScreenTo(SceneState, scene.getFinalScore(), scene.getFinalCoins(), scene.getFinalLevel());
		}

	}
	else {
		mainScreen.update(deltaTime);
		inGameScreen = mainScreen.goToGame();
		if (inGameScreen) scene.initNewLevel(1, true);
	}
	
	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (inGameScreen) scene.render();
	else mainScreen.render();
}

void Game::keyPressed(int key)
{
	if(key == 27) // Escape code
		bPlay = false;
	keys[key] = true;
}

void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Game::specialKeyReleased(int key)
{
	specialKeys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return keys[key];
}

bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}





