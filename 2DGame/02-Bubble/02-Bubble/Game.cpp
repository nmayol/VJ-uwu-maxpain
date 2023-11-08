#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"
#include "SoundController.h"


void Game::init()
{
	SoundController::instance()->init();
	SoundController::instance()->play(JUMP);
	bPlay = true;
	inGameScreen = false;
	glClearColor(0.390625f, 0.58203125f, 0.92578125f, 1.0f); // Blue sky color
	mainScreen.init();
	scene.init(1);
}

bool Game::update(int deltaTime)
{
	if (inGameScreen) scene.update(deltaTime);
	else {
		mainScreen.update(deltaTime);
		inGameScreen = mainScreen.goToGame();
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
	if (key == 27) // Escape code
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

void Game::setInGameScreen(bool inGame) {
	inGameScreen = inGame;
}




