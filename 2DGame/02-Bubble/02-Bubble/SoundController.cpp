#include "SoundController.h"

SoundController* SoundController::singleton;

void SoundController::init()
{
	engine = createIrrKlangDevice();
	sounds[POWERUP] = engine->addSoundSourceFromFile("sounds/Powerup.wav");
	sounds[JUMP] = engine->addSoundSourceFromFile("sounds/Jump.wav");

}

void SoundController::play(Sounds id, bool loop) //Loop is optional
{
	//std::cout<<sounds[sound]<<std::endl;
	ISound* sound;
	//if (id == MENUU || id == MAINTHEME) sound = engine->play2D(sounds[id], true, false, true);
	//else 
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