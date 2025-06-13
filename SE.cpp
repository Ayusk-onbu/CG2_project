#include "SE.h"

void SE::LoadWAVE(const char* filename) {
	soundWaveData_ = SoundLoadWave(filename);
}

void SE::SetAudioBuf() {
#pragma region 再生する波形データの設定
	buf_.pAudioData = soundWaveData_.pBuffer;
	buf_.AudioBytes = soundWaveData_.bufferSize;
	buf_.Flags = XAUDIO2_END_OF_STREAM;
#pragma endregion
}

void SE::PlayWave() {
	PlayAudioWAVE(buf_);
}