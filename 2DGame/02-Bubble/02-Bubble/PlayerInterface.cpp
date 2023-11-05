#include "PlayerInterface.h"

#define Y_SPRITE_OFFSET (1.f / 3.f)
#define X_SPRITE_OFFSET (1.f / 16.f)


PlayerInterface::PlayerInterface() {}

PlayerInterface::~PlayerInterface()
{
	if (backgroundSprite != NULL)
		delete backgroundSprite;

	for (Sprite* number : time) delete number;
	for (Sprite* number : coins) delete number;
	for (Sprite* number : score) delete number;
	if (coinSymbol != NULL)
		delete coinSymbol;
}

void PlayerInterface::init(ShaderProgram& shaderProgram)
{
	total_score = 0;
	total_coins = 0;
	time_left = 400;
	actual_level = 1;

	countdown_frames = 0;
	tick_rate = 24;
	time_counting_down = true;

	//NUMBERS INIT
	lettersSpritesheet.loadFromFile("images/screens/text.png", TEXTURE_PIXEL_FORMAT_RGBA);
	lettersSpritesheet.setMagFilter(GL_NEAREST);
	initNumberInScreen(score, 7, 0, shaderProgram);
	initNumberInScreen(coins, 2, 0, shaderProgram);
	initNumberInScreen(time, 3, 400, shaderProgram);

	//LEVEL
	levelSymbol = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(X_SPRITE_OFFSET, Y_SPRITE_OFFSET), &lettersSpritesheet, &shaderProgram);
	levelSymbol->setNumberAnimations(10);
	for (int j = 0; j < 10; ++j) {
		levelSymbol->setAnimationSpeed(j, 8);
		levelSymbol->addKeyframe(j, glm::vec2(X_SPRITE_OFFSET * j, 0.f));
	}
	levelSymbol->changeAnimation(actual_level);

	//COIN SYMBOL
	coinSymbol = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(X_SPRITE_OFFSET, Y_SPRITE_OFFSET), &lettersSpritesheet, &shaderProgram);
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


	//Background of User Interface 
	background.loadFromFile("images/screens/playerInterface.png", TEXTURE_PIXEL_FORMAT_RGBA);
	background.setMagFilter(GL_NEAREST);
	backgroundSprite = Sprite::createSprite(glm::ivec2(256, 32), glm::vec2(1.f, 1.f), &background, &shaderProgram);
	backgroundSprite->setNumberAnimations(1);
	backgroundSprite->setAnimationSpeed(0, 8);
	backgroundSprite->addKeyframe(0, glm::vec2(0.f, 0.f));
	backgroundSprite->changeAnimation(0);


}

void PlayerInterface::initNumberInScreen(vector<Sprite*>& sprite_vector, const int& n_digits, int number, ShaderProgram& shaderProgram) {
	for (int i = 0; i < n_digits; ++i) {
		Sprite* new_Number = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(X_SPRITE_OFFSET, Y_SPRITE_OFFSET), &lettersSpritesheet, &shaderProgram);
		new_Number->setNumberAnimations(10);

		for (int j = 0; j < 10; ++j) {
			new_Number->setAnimationSpeed(j, 8);
			new_Number->addKeyframe(j, glm::vec2(X_SPRITE_OFFSET * j, 0.f));
		}

		new_Number->changeAnimation(number % 10);
		sprite_vector.push_back(new_Number);
		number = number / 10;
	}
}

// update either score, time or number of coins
void PlayerInterface::updateNumber(vector<Sprite*>& number_in_game, int old_number, int new_number) {
	for (int i = 0; i < number_in_game.size(); ++i) {
		
		if (old_number == new_number) return; // break out early if already finished
		number_in_game[i]->changeAnimation(new_number % 10);
		old_number = old_number / 10;
		new_number = new_number / 10;
	}
}

void PlayerInterface::update(int deltaTime)
{
	coinSymbol->update(deltaTime);
	if (time_counting_down) {
		if (time_left != 0 && countdown_frames > tick_rate) {
			updateNumber(time, time_left, time_left - 1);
			time_left--;
			countdown_frames = 0;
		}
		else if (time_left == 0) time_counting_down = false;
		else countdown_frames++;
	}
}

void PlayerInterface::render()
{
	backgroundSprite->render();
	coinSymbol->render();
	levelSymbol->render();
	for (Sprite* digit : score) digit->render();
	for (Sprite* digit : coins) digit->render();
	for (Sprite* digit : time) digit->render();
}

void PlayerInterface::setScreenXandY(const float& new_x, const float& new_y)
{
	backgroundSprite->setPosition(glm::vec2(new_x, new_y + 41.f));
	coinSymbol->setPosition(glm::vec2(new_x + 88.f, new_y + 65.f));
	levelSymbol->setPosition(glm::vec2(new_x + 168.f, new_y + 65.f));
	
	int i = 0;
	for (Sprite* digit : score) {
		digit->setPosition(glm::vec2(new_x + 64.f - i * 8.f, new_y + 65.f));
		++i;
	}

	i = 0;
	for (Sprite* digit : coins) {
		digit->setPosition(glm::vec2(new_x + 112.f - i * 8.f, new_y + 65.f));
		++i;
	}
	
	i = 0;
	for (Sprite* digit : time) {
		digit->setPosition(glm::vec2(new_x + 224.f - i * 8.f, new_y + 65.f));
		++i;
	}
}

void PlayerInterface::changeActualLevel(const int& new_level) 
{
	levelSymbol->changeAnimation(new_level);
}

void PlayerInterface::addToScore(const int& new_score)
{
	updateNumber(score, total_score, total_score + new_score);
	total_score = total_score;
}

void PlayerInterface::addCoins(const int& amount)
{
	updateNumber(coins, total_coins, (total_coins + amount) % 100);
	total_coins = (total_coins + amount) % 100;
}


void PlayerInterface::startTime()
{
	updateNumber(time, time_left, 400);
	time_left = 400;
	tick_rate = 24;
	time_counting_down = true;
}

void PlayerInterface::stopTime()
{
	time_counting_down = false;
}

bool PlayerInterface::endedTime()
{
	return time_left == 0;
}

void PlayerInterface::changeTickRate(const int& new_tick_rate)
{
	tick_rate = new_tick_rate;
}