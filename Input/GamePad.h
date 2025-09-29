#pragma once
#include <windows.h>
#include <Xinput.h>

class GamePad {
public:
    GamePad(int index = 0);  // コントローラー番号（0〜3）
    ~GamePad() = default;

    void Update();                 // 入力情報を更新
    bool IsConnected() const;     // 接続状態
    bool IsPressed(WORD button);  // ボタン入力判定（A/B/Xなど）

    BYTE GetLeftTrigger() const;
    SHORT GetLeftStickX() const;
    SHORT GetLeftStickY() const;

    BYTE GetRightTrigger() const;  // 右トリガーの値
	SHORT GetRightStickX() const; // 右スティックのX軸
	SHORT GetRightStickY() const; // 右スティックのY軸
	

private:
    int controllerIndex_;
    XINPUT_STATE state_;
    bool isConnected_;
};
