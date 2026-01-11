#pragma once
#include <xaudio2.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <cassert>
#include <wrl.h>
#include <vector>

struct ChunkHeader {
	char id[4]; // チャンクの識別子（4文字）
	int32_t size; // チャンクのサイズ（ヘッダーを除く）
};
struct RiffHeader {
	ChunkHeader chunk;// "RIFF"
	char type[4];// "WAVE"
};
struct FormatChunk {
	ChunkHeader chunk;// "fmt "
	WAVEFORMATEX fmt; // WAVEフォーマット情報
};

struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};

class Audio {
public:
	static void Initialize();// XAudio2の初期化
	static void Finalize();
protected:
	// 読み込み関数
	static SoundData LoadWave(const std::string& filename);
protected:
	static Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	static IXAudio2MasteringVoice* masterVoice_;
};

class MediaAudioDecoder
{
	
public:

	static const SoundData DecodeAudioFile(const std::wstring& filePath);

private:
	Microsoft::WRL::ComPtr<IMFSourceReader> sourceReader;
	WAVEFORMATEX waveFormat;

};

