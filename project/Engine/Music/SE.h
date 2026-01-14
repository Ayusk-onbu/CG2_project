#pragma once
#include "Audio.h"

class SE
	: public Audio
{
public:
	void Unload() {
		// バッファメモリの開放
		delete[] data_.pBuffer;
		data_.pBuffer = 0;
		data_.bufferSize = 0;
		data_.wfex = {};
	}
public:
	void LoadWAVE(const std::string& filename);
	void Play();
public:
	void SetVolume(float volume);
private:
	SoundData data_;
	float volume_ = 1.0f;
};

