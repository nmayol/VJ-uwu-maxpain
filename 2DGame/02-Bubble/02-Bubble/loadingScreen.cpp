#include "loadingScreen.h"

#define Y_SPRITE_OFFSET (1.f / 3.f)
#define X_SPRITE_OFFSET (1.f / 16.f)

loadingScreen::loadingScreen(){}

loadingScreen::~loadingScreen()
{
	if (backgroundSprite != NULL)
		delete backgroundSprite;

	if (levelSymbol != NULL)
		delete levelSymbol;
}

void loadingScreen::init(ShaderProgram& shaderProgram, const int& level_number, const int& number_lives)
{
	//Background of Screen
	background.loadFromFile("images/screens/loadingScreen.png", TEXTURE_PIXEL_FORMAT_RGBA);
	background.setMagFilter(GL_NEAREST);
	backgroundSprite = Sprite::createSprite(glm::ivec2(256, 240), glm::vec2(1.f, 1.f), &background, &shaderProgram);
	backgroundSprite->setNumberAnimations(1);
	backgroundSprite->setAnimationSpeed(0, 8);
	backgroundSprite->addKeyframe(0, glm::vec2(0.f, 0.f));
	backgroundSprite->changeAnimation(0);
	backgroundSprite->setPosition(glm::vec2(0.f, 41.f));

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

void loadingScreen::update_level(const int& level_number, const int& number_lives)
{
	levelSymbol->changeAnimation(level_number);
	livesSymbol->changeAnimation(number_lives);
}

void loadingScreen::render()
{
	backgroundSprite->render();
	levelSymbol->render();
	livesSymbol->render();
}
