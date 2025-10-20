#include "Key.h"
#include <cassert>
#include <iostream>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Key::Initialize(Microsoft::WRL::ComPtr<IDirectInput8> directInput, HWND hwnd) {

	HRESULT result;
	// キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);// GUIDで他にも設定
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);// 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		// 画面が手前にある場合のみ | デバイスをこのアプリのみで占有しない | Windowsキーの無効
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Key::Update() {
	memcpy(preKeys_.data(), keys_.data(), sizeof(keys_));
	// キーボード情報の取得開始
	Key::IsAcquire();
	// 全キーの入力状態を取得
	keyboard->GetDeviceState(sizeof(keys_), keys_.data());
}

void Key::IsAcquire() {
	// 接続が出来ているかどうか
	if (FAILED(keyboard->Acquire())) {
		// 接続できていなかった場合エラーを出す
		std::cerr << "Failed to acquire keyboard device." << std::endl;
	}
}