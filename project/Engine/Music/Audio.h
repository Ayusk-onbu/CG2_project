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

class Audio
{
public:
	//struct ChunkHeader {
	//	char id[4]; // チャンクの識別子（4文字）
	//	int32_t size; // チャンクのサイズ（ヘッダーを除く）
	//};
	//struct RiffHeader {
	//	ChunkHeader chunk;// "RIFF"
	//	char type[4];// "WAVE"
	//};
	//struct FormatChunk {
	//	ChunkHeader chunk;// "fmt "
	//	WAVEFORMATEX fmt; // WAVEフォーマット情報
	//};

	//struct SoundData {
	//	// 波形フォーマット
	//	WAVEFORMATEX wfex;
	//	// バッファの先頭アドレス
	//	BYTE* pBuffer;
	//	// バッファのサイズ
	//	unsigned int bufferSize;
	//};
protected:

	/// <summary>
	/// create
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	SoundData SoundLoadWave(const char* filename);

public:

	static void SoundUnload(SoundData* soundData) {
		xAudio2_.Reset();
		// バッファメモリの開放
		delete[] soundData->pBuffer;

		soundData->pBuffer = 0;
		soundData->bufferSize = 0;
		soundData->wfex = {};
	}

	/// <summary>
	/// 鳴らす
	/// </summary>
	/// <param name="xAudio2"></param>
	/// <param name="soundData"></param>
	void SoundPlayWave(const SoundData& soundData);

	/// <summary>
	/// 指揮者とスピーカーの用意一回でいい
	/// </summary>
	static void Create();
	
#pragma region Wavファイルの初期化(InitializeWave)
	/// <summary>
	/// フォーマットによってそのフォーマット専用のプレイヤーを作る
	/// (例 CD→CDプレイヤー　レコード→レコードプレイヤー)
	/// </summary>
	/// <param name="pSourceVoice"></param>
	/// <param name="soundData"></param>
	void InitializeWave(const SoundData& soundData);

	/// <summary>
	/// フォーマットによってそのフォーマット専用のプレイヤーを作る
	/// (例 CD→CDプレイヤー　レコード→レコードプレイヤー)
	/// </summary>
	static void InitializeWave();
#pragma endregion
	
	///// <summary>
	///// かけたい音データをセットする
	///// </summary>
	///// <param name="soundData"></param>
	///// <param name="buf"></param>
	//void SetAudioBuf(const SoundData& soundData, XAUDIO2_BUFFER& buf);

	//void SetAudioBuf(const SoundData& soundData);

	void PlayAudioWAVE(XAUDIO2_BUFFER& buf);

#pragma region MF
	
#pragma endregion

private:
	 
	static WAVEFORMATEX MakeWaveFmt();

public:
	// 指揮者
	static Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	// スピーカー
	static IXAudio2MasteringVoice* masterVoice_;// これはComptr無理
	// mp3プレイヤー
	static IXAudio2SourceVoice* pSourceVoiceMP3_;
	// wavプレイヤー
	static IXAudio2SourceVoice* pSourceVoiceWAVE_;
};

class MediaAudioDecoder
{
	
public:

	static const SoundData DecodeAudioFile(const std::wstring& filePath);

private:
	Microsoft::WRL::ComPtr<IMFSourceReader> sourceReader;
	WAVEFORMATEX waveFormat;

};

