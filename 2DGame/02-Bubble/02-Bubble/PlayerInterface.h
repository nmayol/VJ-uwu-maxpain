#pragma once
#include "Sprite.h"

class PlayerInterface
{
public:
	PlayerInterface();
	~PlayerInterface();

	void init(ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();
	void reset();
	void setScreenXandY(const float& new_x, const float& new_y);

	void changeActualLevel(const int& new_level);
	void addToScore(const int& new_score);
	int getTotalScore();
	void addCoins(const int& amount);
	int getTotalCoins();

	int getTime();

	void startTime();
	void stopTime();
	void changeTickRate(const int& new_tick_rate);
	void setTimeToNone();
	bool endedTime();


private:
	Sprite* backgroundSprite;
	Texture background;

	vector<Sprite*> score;
	vector<Sprite*> coins;
	Sprite* coinSymbol;
	Sprite* levelSymbol;
	vector<Sprite*> time;
	Texture lettersSpritesheet;

	int total_score;
	int total_coins;
	int actual_level;
	int time_left;

	bool time_counting_down;
	int countdown_frames;
	int tick_rate;

	float actual_Y_player;

	void initNumberInScreen(vector<Sprite*>& sprite_vector, const int& n_digits, int number, ShaderProgram& shaderProgram);
	void updateNumber(vector<Sprite*>& number_in_game, int old_number, int new_number);
};
