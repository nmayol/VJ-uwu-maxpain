#include "SoundController.h"

SoundController* SoundController::singleton;

void SoundController::init()
{
	engine = createIrrKlangDevice();
	sounds[POWERUP] = engine->addSoundSourceFromFile("sounds/Powerup.wav");
	sounds[JUMP] = engine->addSoundSourceFromFile("sounds/Jump.wav");
	sounds[LEVEL1] = engine->addSoundSourceFromFile("sounds/Level1.wav");
	sounds[LEVEL2] = engine->addSoundSourceFromFile("sounds/Level2.wav");
	sounds[FLAG] = engine->addSoundSourceFromFile("sounds/Flagpole.wav");
	sounds[DEAD] = engine->addSoundSourceFromFile("sounds/Die.wav");
	sounds[COIN] = engine->addSoundSourceFromFile("sounds/Coin.wav");
	sounds[GAMEOVER] = engine->addSoundSourceFromFile("sounds/Game Over.wav");
	sounds[WARP] = engine->addSoundSourceFromFile("sounds/Warp.wav");
	sounds[BREAK] = engine->addSoundSourceFromFile("sounds/Break.wav");
	sounds[UNDERWORLD] = engine->addSoundSourceFromFile("sounds/Underworld.wav");
	sounds[WIN] = engine->addSoundSourceFromFile("sounds/Win.wav");
	sounds[KILL] = engine->addSoundSourceFromFile("sounds/Squish.wav");
	sounds[SKID] = engine->addSoundSourceFromFile("sounds/Skid.wav");
	sounds[STARRED] = engine->addSoundSourceFromFile("sounds/Star.wav");



}

void SoundController::play(Sounds id, bool loop) //Loop is optional
{
	//std::cout<<sounds[sound]<<std::endl;
	ISound* sound;
	if (id == LEVEL1 || id == LEVEL2 || id == UNDERWORLD) sound = engine->play2D(sounds[id], true, false, true);
	else 
		sound = engine->play2D(sounds[id], loop, false, true);
	playing[id].push_back(sound);
}

void SoundController::stop(Sounds id)
{
	if (playing.find(id) == playing.end()) return;
	for (ISound* sound : playing[id])
	{
		sound->stop();
	}
}

void SoundController::stopAll()
{
	engine->stopAllSounds();
}

void SoundController::pauseAll()
{
	engine->setAllSoundsPaused(true);
}

void SoundController::unPauseAll()
{
	engine->setAllSoundsPaused(false);
}

bool SoundController::isPlaying(Sounds sound)
{
	return playing.find(sound) != playing.end();
}

void SoundController::OnSoundStopped(ISound* sound, E_STOP_EVENT_CAUSE reason, void* userData)
{
	for (auto list : playing)
	{
		for (ISound* s : list.second)
		{
			if (s == sound) list.second.remove(s);
		}
		if (list.second.empty())
			playing.erase(list.first);
	}
}