#pragma once
#pragma region Input
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma endregion
#include <wrl.h>
#include <array>
class Key
{
public:
#pragma region 初期化(Initialize)
	/// <summary>
	/// キー情報について初期化
	/// </summary>
	void Initialize(Microsoft::WRL::ComPtr<IDirectInput8> directInput,HWND hwnd);
#pragma endregion
#pragma region 更新(Update)
	/// <summary>
	/// キー入力の更新
	/// </summary>
	void Update();
#pragma endregion
private:
#pragma region デバイスの接続確認(IsAcquire)
	/// <summary>
	/// デバイスが適切に取得されているか
	/// </summary>
	/// <returns></returns>
	void IsAcquire();
#pragma endregion
#pragma region キー情報のコピー(GetAllKey)
	const std::array<BYTE, 256>& GetAllKey() const { return keys_; }
#pragma endregion
public:
	bool PressKey(const int& keyID) { return keys_[keyID]; }
	bool PressedKey(const int& keyID) { return keys_[keyID] && !preKeys_[keyID]; }
	bool HoldKey(const int& keyID) { return keys_[keyID] && preKeys_[keyID]; }
	bool ReleaseKey(const int& keyID) { return !keys_[keyID] && preKeys_[keyID]; }
private:
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard = nullptr;
	std::array<BYTE, 256> keys_;
	std::array<BYTE, 256> preKeys_ = { 0 };
};

