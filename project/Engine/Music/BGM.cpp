#include "BGM.h"

void BGM::Play(const std::string& filename) {
	data_ = Audio::LoadWave(filename);
	// 再生
	HRESULT result;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice_, &data_.wfex);
	assert(SUCCEEDED(result));
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = data_.pBuffer;
	buf.AudioBytes = data_.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	result = pSourceVoice_->SubmitSourceBuffer(&buf);
	pSourceVoice_->SetVolume(volume_);
	result = pSourceVoice_->Start();
}

void BGM::Stop() {
	HRESULT result;
	result = pSourceVoice_->Stop();
}

void BGM::SetVolume(float volume) {
	volume_ = volume;
}