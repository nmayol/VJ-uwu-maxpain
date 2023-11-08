#include "loadingScreen.h"

#define Y_SPRITE_OFFSET (1.f / 3.f)
#define X_SPRITE_OFFSET (1.f / 16.f)

loadingScreen::loadingScreen(){}

loadingScreen::~loadingScreen()
{
	if (loadingBackgroundSprite != NULL)
		delete loadingBackgroundSprite;

	if (timeoutBackgroundSprite != NULL)
		delete timeoutBackgroundSprite;

	if (levelSymbol != NULL)
		delete levelSymbol;
}

void loadingScreen::init(ShaderProgram& shaderProgram, const int& level_number, const int& number_lives)
{

	renderTimeout = false;

	//Background of Screen
	loadingAndTimeoutbackground.loadFromFile("images/screens/loading&timeoutScreen.png", TEXTURE_PIXEL_FORMAT_RGBA);
	loadingAndTimeoutbackground.setMagFilter(GL_NEAREST);

	//loading Screen
	loadingBackgroundSprite = Sprite::createSprite(glm::ivec2(256, 240), glm::vec2(0.5f, 1.f), &loadingAndTimeoutbackground, &shaderProgram);
	loadingBackgroundSprite->setNumberAnimations(1);
	loadingBackgroundSprite->setAnimationSpeed(0, 8);
	loadingBackgroundSprite->addKeyframe(0, glm::vec2(0.f, 0.f));
	loadingBackgroundSprite->changeAnimation(0);
	loadingBackgroundSprite->setPosition(glm::vec2(0.f, 41.f));

	//timeout Screen
	timeoutBackgroundSprite = Sprite::createSprite(glm::ivec2(256, 240), glm::vec2(0.5f, 1.f), &loadingAndTimeoutbackground, &shaderProgram);
	timeoutBackgroundSprite->setNumberAnimations(1);
	timeoutBackgroundSprite->setAnimationSpeed(0, 8);
	timeoutBackgroundSprite->addKeyframe(0, glm::vec2(0.5f, 0.f));
	timeoutBackgroundSprite->changeAnimation(0);
	timeoutBackgroundSprite->setPosition(glm::vec2(0.f, 41.f));

	//LEVEL
	levelSymbolSpritesheet.loadFromFile("images/screens/text.png", TEXTURE_PIXEL_FORMAT_RGBA);
	levelSymbolSpritesheet.setMagFilter(GL_NEAREST);
	levelSymbol = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(X_SPRITE_OFFSET, Y_SPRITE_OFFSET), &levelSymbolSpritesheet, &shaderProgram);
	levelSymbol->setNumberAnimations(10);
	for (int j = 0; j < 10; ++j) {
		levelSymbol->setAnimationSpeed(j, 8);
		levelSymbol->addKeyframe(j, glm::vec2(X_SPRITE_OFFSET * j, 0.f));
	}
	levelSymbol->changeAnimation(level_number);
	levelSymbol->setPosition(glm::vec2(152.f, 121.f));

	//LIVES
	livesSymbol = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(X_SPRITE_OFFSET, Y_SPRITE_OFFSET), &levelSymbolSpritesheet, &shaderProgram);
	livesSymbol->setNumberAnimations(10);
	for (int j = 0; j < 10; ++j) {
		livesSymbol->setAnimationSpeed(j, 8);
		livesSymbol->addKeyframe(j, glm::vec2(X_SPRITE_OFFSET * j, 0.f));
	}
	livesSymbol->changeAnimation(number_lives);
	livesSymbol->setPosition(glm::vec2(136.f, 153.f));
}

void loadingScreen::update(const int& level_number, const int& number_lives)
{
	levelSymbol->changeAnimation(level_number);
	livesSymbol->changeAnimation(number_lives);
}

void loadingScreen::setTimeoutScreen()
{
	renderTimeout = true;
}

void loadingScreen::setLoadingScreen()
{
	renderTimeout = false;
}

void loadingScreen::render()
{
	if (renderTimeout) timeoutBackgroundSprite->render();
	else {
		loadingBackgroundSprite->render();
		levelSymbol->render();
		livesSymbol->render();
	}
}
