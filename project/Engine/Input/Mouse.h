#pragma once
#pragma region Input
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
//#pragma comment(lib,"dinput8.lib")
//#pragma comment(lib,"dxguid.lib")
#pragma endregion
#include <wrl.h>
#include "Vector2.h"
#include <unordered_map>
class Mouse
{
public:
	/// <summary>
	/// マウスの初期化
	/// </summary>
	/// <param name="directInput"></param>
	/// <param name="hwnd"></param>
	void Initialize(Microsoft::WRL::ComPtr<IDirectInput8> directInput, HWND hwnd);

	/// <summary>
	/// マウスの更新
	/// </summary>
	void Update();
private:

	/// <summary>
	/// デバイスが適切に取得されているか
	/// </summary>
	/// <returns></returns>
	void IsAcquire();
	
	/// <summary>
	/// マウスの情報を取得
	/// </summary>
	/// <returns></returns>
	void CopyAllMouse(DIMOUSESTATE2* mouse) { mouse = &mouseState_; }
public:
	/// <summary>
	/// マウスのポジションを取得
	/// </summary>
	/// <param name="pos"></param>
	void GetPosition(Vector2& pos);

	/// <summary>
	/// ホイールした量を取得
	/// </summary>
	/// <returns></returns>
	float GetWheel();
	
	/// <summary>
	/// ０なら左クリック １なら右クリック 2なら真ん中クリック
	/// </summary>
	bool IsButtonPress(int buttonIndex);

	/// <summary>
	/// ０なら左クリック １なら右クリック 2なら真ん中クリック
	/// </summary>
	bool IsButtonRelease(int buttonIndex);

	/// <summary>
	/// マウスの動いた量を取得
	/// </summary>
	/// <returns></returns>
	Vector2 getDelta();

	/// <summary>
	/// 何かボタンを押しているときにドラッグ量を取得
	/// </summary>
	/// <param name="buttonIndex"></param>
	/// <returns></returns>
	bool isDragging(int buttonIndex);

	//bool isDoubleClick(int buttonIndex);
	//bool isHovering(Rect area);
	//bool isSwipe(Vector2 direction);

private:
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse = nullptr;
	DIMOUSESTATE2 mouseState_;
	DIMOUSESTATE2 preMouseState_;
};

