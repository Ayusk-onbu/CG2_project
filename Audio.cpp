#include "Audio.h"
#include "iostream"

#pragma comment(lib,"xaudio2.lib")

// 指揮者
Microsoft::WRL::ComPtr<IXAudio2> Audio::xAudio2_;
// スピーカー
IXAudio2MasteringVoice* Audio::masterVoice_;// これはComptr無理
// mp3プレイヤー
IXAudio2SourceVoice* Audio::pSourceVoiceMP3_ = nullptr;
// wavプレイヤー
IXAudio2SourceVoice* Audio::pSourceVoiceWAVE_ = nullptr;

Audio::SoundData Audio::SoundLoadWave(const char* filename) {
#pragma region ①ファイルオープン
	//HRESULT result;
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルが開けなかったらエラー
	assert(file.is_open());
#pragma endregion
#pragma region ②.wavデータ読み込み
	// ヘッダ情報を読み込む
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFか✅
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// ファイルがWAVEか✅
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	// Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
#pragma endregion
#pragma region ③ファイルクローズ
	file.close();
#pragma endregion
#pragma region ④読み込んだ音声データを return
	// returnするための音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
#pragma endregion
}

void Audio::SoundPlayWave(const SoundData& soundData) {
#pragma region 波形フォーマアットをもとにSourceVoiceの生成
	HRESULT result;
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));
#pragma endregion
#pragma region 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;// 
	buf.AudioBytes = soundData.bufferSize;// 
	buf.Flags = XAUDIO2_END_OF_STREAM;// 設定したい
#pragma endregion
#pragma region 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);// 同じFmtならここで音を変える
	//result = pSourceVoice->SetVolume(1.0f); // 音量を設定
	result = pSourceVoice->Start();
#pragma endregion
}

#pragma region InitializeWave
void Audio::InitializeWave(const SoundData& soundData) {
#pragma region 波形フォーマアットをもとにSourceVoiceの生成
	HRESULT result;
	result = xAudio2_->CreateSourceVoice(&pSourceVoiceWAVE_, &soundData.wfex);
	assert(SUCCEEDED(result));
#pragma endregion
}

void Audio::InitializeWave() {
#pragma region 波形フォーマットをもとにSourceVoiceの生成
	HRESULT result;
	WAVEFORMATEX wfex{};
	wfex = MakeWaveFmt();
	result = xAudio2_->CreateSourceVoice(&pSourceVoiceWAVE_, &wfex);
	assert(SUCCEEDED(result));
#pragma endregion
}
#pragma endregion

void Audio::PlayAudioWAVE(XAUDIO2_BUFFER& buf) {
#pragma region 波形データの再生
	HRESULT result;
	result = pSourceVoiceWAVE_->SubmitSourceBuffer(&buf);// 同じFmtならここで音を変える
	//result = pSourceVoiceWAVE->SetVolume(1.0f); // 音量を設定
	result = pSourceVoiceWAVE_->Start();
#pragma endregion
}

void Audio::Create() {
	HRESULT hr;
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

WAVEFORMATEX Audio::MakeWaveFmt() {
	WAVEFORMATEX wfex{};
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nChannels = 2; // ステレオ
	wfex.nSamplesPerSec = 44100; // 44.1kHz
	wfex.nAvgBytesPerSec = 44100 * 2 * sizeof(short);
	wfex.nBlockAlign = 2 * sizeof(short);
	wfex.wBitsPerSample = 16; // 16bit
	wfex.cbSize = 0;

	return wfex;
}
