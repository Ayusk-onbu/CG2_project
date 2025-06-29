#pragma once
#include "Audio.h"

class SE
	: public Audio
{
public:
	void LoadWAVE(const char* filename);
	void SetAudioBuf();
	void PlayWave();
private:
	SoundData soundWaveData_;
	XAUDIO2_BUFFER buf_;
};

