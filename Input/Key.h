#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#include <array>

class Key
{
public:
	void Initialize(Microsoft::WRL::ComPtr<IDirectInput8> directInput,HWND hwnd);
	void Update();
private:
	/// <summary>
	/// デバイスが接続されているか
	/// </summary>
	/// <returns></returns>
	void IsAcquire();
public:
	/// <summary>
	/// 押している
	/// </summary>
	/// <param name="keyID"></param>
	/// <returns></returns>
	bool PressKey(const int& keyID) { return keys_[keyID]; }

	/// <summary>
	/// 今押した
	/// </summary>
	/// <param name="keyID"></param>
	/// <returns></returns>
	bool PressedKey(const int& keyID) { return keys_[keyID] && !preKeys_[keyID]; }

	/// <summary>
	/// ずっと押している
	/// </summary>
	/// <param name="keyID"></param>
	/// <returns></returns>
	bool HoldKey(const int& keyID) { return keys_[keyID] && preKeys_[keyID]; }

	/// <summary>
	/// 離した
	/// </summary>
	/// <param name="keyID"></param>
	/// <returns></returns>
	bool ReleaseKey(const int& keyID) { return !keys_[keyID] && preKeys_[keyID]; }
public:
	const std::array<BYTE, 256>& GetAllKey() const { return keys_; }
private:
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard = nullptr;
	std::array<BYTE, 256> keys_;
	std::array<BYTE, 256> preKeys_ = { 0 };
};

