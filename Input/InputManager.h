#pragma once
#include "Key.h"
#include "Mouse.h"
#include "GamePad.h"

class InputManager
{
public:
#pragma region 初期化(Initialize)
	/// <summary>
	/// Key, の初期化
	/// </summary>
	void Initialize(WNDCLASS& wc, HWND hwnd);
#pragma endregion
#pragma region 更新(Update)
	/// <summary>
	/// Key, の更新
	/// </summary>
	void Update();
#pragma endregion
public: 
	static Key& GetKey() { return key_; }
	static Mouse& GetMouse(){ return mouse_; }
	static GamePad& GetGamePad(int index){return gamePad_[index];}
private:
	Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;
	static Key  key_;
	static Mouse mouse_;
	static GamePad gamePad_[4]; // 最大4つのゲームパッドをサポート
};

