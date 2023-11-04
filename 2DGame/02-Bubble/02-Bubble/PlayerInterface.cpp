#include "PlayerInterface.h"

PlayerInterface::PlayerInterface()
{
	Texture background = Texture();
}

PlayerInterface::~PlayerInterface()
{
	if (backgroundSprite != NULL)
		delete backgroundSprite;
}

void PlayerInterface::init(ShaderProgram& shaderProgram)
{
	background.loadFromFile("images/screens/playerInterface.png", TEXTURE_PIXEL_FORMAT_RGBA);
	background.setMagFilter(GL_NEAREST);
	backgroundSprite = Sprite::createSprite(glm::ivec2(255, 32), glm::vec2(1.f, 1.f), &background, &shaderProgram);

	backgroundSprite->setNumberAnimations(1);
	backgroundSprite->setAnimationSpeed(0, 8);
	backgroundSprite->addKeyframe(0, glm::vec2(0.f, 0.f));

	backgroundSprite->changeAnimation(0);
}

void PlayerInterface::update(int deltaTime)
{
	backgroundSprite->update(deltaTime);
}

void PlayerInterface::render()
{
	backgroundSprite->render();
}

void PlayerInterface::setScreenX(const float& new_x)
{
	backgroundSprite->setPosition(glm::vec2(new_x, 41.f));
}
