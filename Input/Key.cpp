#include "Key.h"
#include <cassert>
#include <iostream>

#pragma region 初期化
void Key::Initialize(Microsoft::WRL::ComPtr<IDirectInput8> directInput, HWND hwnd) {

	HRESULT result;
#pragma region キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);// GUIDで他にも設定
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);// 標準形式
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		// 画面が手前にある場合のみ | デバイスをこのアプリのみで占有しない | Windowsキーの無効
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
#pragma endregion
}
#pragma endregion

#pragma region 更新
void Key::Update() {
	memcpy(preKeys_.data(), keys_.data(), sizeof(keys_));
	// キーボード情報の取得開始
	Key::IsAcquire();
	// 全キーの入力状態を取得
	keyboard->GetDeviceState(sizeof(keys_), keys_.data());
}
#pragma endregion

void Key::IsAcquire() {
#pragma region 接続出来ているかどうかの確認
	if (FAILED(keyboard->Acquire())) {
		std::cerr << "Failed to acquire keyboard device." << std::endl;
	}
#pragma endregion
}