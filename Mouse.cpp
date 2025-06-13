#include "Mouse.h"
#include <cassert>
#include <iostream>
#include "ImGuiManager.h"

void Mouse::Initialize(Microsoft::WRL::ComPtr<IDirectInput8> directInput, HWND hwnd) {
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

	CopyState();

	Mouse::IsAcquire();
	// マウス情報の取得開始
	mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState_);
	ImGui::Begin("preStateAfter");
	ImGui::Text("preOne : %d", preState[0]);
	ImGui::Text("preTwo : %d", preState[1]);
	ImGui::Text("preThree : %d", preState[2]);
	ImGui::End();
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

bool Mouse::IsButtonPress(int buttonIndex) {
	//return (mouseState_.rgbButtons[buttonIndex] & 0x80);
	return (GetAsyncKeyState(MouseButtonMap.at(static_cast<Mouse::Type>(buttonIndex))) & 0x8000) != 0;
}

bool Mouse::IsButtonRelease(int button) {
	ImGui::Begin("IsButtonRelease");
	ImGui::Text("boolPre : %d", ((preState[button] == true) && IsButtonPress(button) == false));
	ImGui::End();
	bool Is = ((preState[button] == 1) && IsButtonPress(button) == 0);
	return Is;
}

void Mouse::CopyState() {
	if (IsButtonPress(0)) {
		preState[0] = true;
	}
	else {
		preState[0] = false;
	}
	if (IsButtonPress(1)) {
		preState[1] = true;
	}
	else {
		preState[1] = false;
	}
	if (IsButtonPress(2)) {
		preState[2] = true;
	}
	else {
		preState[2] = false;
	}
	ImGui::Begin("preState");
	ImGui::Text("preOne : %d", preState[0]);
	ImGui::Text("preTwo : %d", preState[1]);
	ImGui::Text("preThree : %d", preState[2]);
	ImGui::End();
}