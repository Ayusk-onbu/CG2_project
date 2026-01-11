#include "SE.h"

void SE::LoadWAVE(const std::string& filename) {
	// 音データを読み込み、保存する
	data_ = Audio::LoadWave(filename);
}

void SE::Play() {
	// 再生
	HRESULT result;
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &data_.wfex);
	assert(SUCCEEDED(result));
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = data_.pBuffer;
	buf.AudioBytes = data_.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = XAUDIO2_NO_LOOP_REGION;
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	pSourceVoice->SetVolume(volume_);
	result = pSourceVoice->Start();
}