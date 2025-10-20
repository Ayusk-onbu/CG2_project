#pragma once
#include "Audio.h"

class BGM
	: public Audio
{
public:
	void LoadWAVE(const char* filename);
	void SetPlayAudioBuf();
	SoundData& GetSoundData() { return soundWaveData_; }
	void Unload() {
		// バッファメモリの開放
		delete[] soundWaveData_.pBuffer;
		soundWaveData_.pBuffer = 0;
		soundWaveData_.bufferSize = 0;
		soundWaveData_.wfex = {};
	}
private:
	SoundData soundWaveData_;
};

