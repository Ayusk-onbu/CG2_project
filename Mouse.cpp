#include "Mouse.h"
#include <cassert>
#include <iostream>

void Mouse::Initialize(Microsoft::WRL::ComPtr<IDirectInput8> directInput, HWND& hwnd) {
	HRESULT result;

#pragma region マウスの生成
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);// GUIDで他にも設定
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region 入力データ形式のセット
	result = mouse->SetDataFormat(&c_dfDIMouse);// 標準形式
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region 排他制御レベルのセット
	result = mouse->SetCooperativeLevel(
		// 画面が手前にある場合のみ | デバイスをこのアプリのみで占有しない | Windowsキーの無効
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
#pragma endregion
}

void Mouse::Update() {
	Mouse::IsAcquire();
	// マウス情報の取得開始
	mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState_);
}

void Mouse::IsAcquire() {
#pragma region 接続出来ているかどうかの確認
	if (FAILED(mouse->Acquire())) {
		std::cerr << "Failed to acquire keyboard device." << std::endl;
	}
#pragma endregion
}

void Mouse::GetPosition(Vector2& pos) {
	POINT cursorPos;
	// スクリーン座標(デスクトップ上の座標)の取得
	GetCursorPos(&cursorPos);
	// ウィンドウ上に変換
	ScreenToClient(GetActiveWindow(), &cursorPos);
	pos.x = static_cast<float>(cursorPos.x) / 1280.0f - 0.5f;
	pos.y = -(static_cast<float>(cursorPos.y) / 720.0f - 0.5f);
}