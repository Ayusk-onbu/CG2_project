#include "BGM.h"

void BGM::LoadWAVE(const char* filename) {
	soundWaveData_ = SoundLoadWave(filename);
}

void BGM::SetPlayAudioBuf() {
#pragma region 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundWaveData_.pBuffer;
	buf.AudioBytes = soundWaveData_.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = XAUDIO2_LOOP_INFINITE;
#pragma endregion
	// 再生
	PlayAudioWAVE(buf);
}

//void BGM::SetAudioBuf(XAUDIO2_BUFFER& buf) {
//#pragma region 再生する波形データの設定
//	buf.pAudioData = soundWaveData_.pBuffer;
//	buf.AudioBytes = soundWaveData_.bufferSize;
//	buf.Flags = XAUDIO2_END_OF_STREAM;
//	buf.LoopCount = XAUDIO2_LOOP_INFINITE;
//#pragma endregion
//}