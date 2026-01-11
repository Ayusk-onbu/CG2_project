#pragma once
#include "Audio.h"

class BGM
	: public Audio
{
public:
	void Play(const std::string& filename);
	void Stop();
	void SetVolume(float volume);
	void Unload() {
		// バッファメモリの開放
		delete[] data_.pBuffer;
		data_.pBuffer = 0;
		data_.bufferSize = 0;
		data_.wfex = {};
	}
private:
	IXAudio2SourceVoice* pSourceVoice_ = nullptr;
	SoundData data_;
	float volume_ = 1.0f;
};

