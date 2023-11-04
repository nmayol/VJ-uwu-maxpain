#include "PlayerInterface.h"

PlayerInterface::PlayerInterface()
{
}

PlayerInterface::~PlayerInterface()
{
	if (backgroundSprite != NULL)
		delete backgroundSprite;
}

void PlayerInterface::init(ShaderProgram& shaderProgram)
{
	background.loadFromFile("images/screens/playerInterface.png", TEXTURE_PIXEL_FORMAT_RGBA);
	backgroundSprite = Sprite::createSprite(glm::ivec2(256, 32), glm::vec2(1.f, 1.f), &background, &shaderProgram);

}

void PlayerInterface::update(int deltaTime)
{

}

void PlayerInterface::render()
{
}
