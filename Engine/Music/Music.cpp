#include "Music.h"

void Music::Initialize() {
	Audio::Create();
	Audio::InitializeWave();
}

void Music::UnLoad() {
	Audio::SoundUnload(&bgm_.GetSoundData());
	//bgm_.Unload();
}