#pragma once
#pragma region Input
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma endregion
#include <wrl.h>
#include "Vector2.h"
class Mouse
{
public:
#pragma region 初期化(Initialize)
	/// <summary>
	/// マウスの初期化
	/// </summary>
	/// <param name="directInput"></param>
	/// <param name="hwnd"></param>
	void Initialize(Microsoft::WRL::ComPtr<IDirectInput8> directInput, HWND& hwnd);
#pragma endregion
#pragma region 更新(Update)
	/// <summary>
	/// マウスの更新
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
#pragma region マウス情報のコピー(GetAllMouse)
	/// <summary>
	/// マウスの情報を取得
	/// 	/// </summary>
	/// 	/// <returns></returns>
	void CopyAllMouse(DIMOUSESTATE2* mouse) { mouse = &mouseState_; }
#pragma endregion

public:
	void GetPosition(Vector2& pos);
private:
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse = nullptr;
	DIMOUSESTATE2 mouseState_;
};

