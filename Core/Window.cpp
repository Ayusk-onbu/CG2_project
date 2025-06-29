#include "Window.h"
#include "ImGuiManager.h"

void Window::SetUpClass(const std::wstring& windowClassName) {
	// ロングポインターファンクションWindow準備に関数をぶちこむ
	wc_.lpfnWndProc = WindowProc;// 関数ポインタを渡してる
	// ウィンドウのクラス名を設定
#pragma region LPCWSTRの説明
	// LPCWSTR(Long Pointer Constant Wide String)は、
	// Windows APIで使用される文字列型の一つで、Unicode文字列を表す
#pragma endregion
	wc_.lpszClassName = windowClassName.c_str();
	// インスタンスハンドル
	SetHInstance();
	// カーソル
	SetCursorShape();
	// ウィンドウクラスを登録する
	RegisterClass(&wc_);
}

void Window::SetUpWindow() {
	// ウィンドウの位置とサイズを設定
	SetUpSize();
	// クライアント領域をもとに実際のサイズにwrcを変更してもらう(タイトルバーを含むサイズを決める)
	AdjustWindowRect(&wrc_, style_, false);
}

void Window::MakeWindow(const std::wstring& titleName) {
	hwnd_ = CreateWindow(
		wc_.lpszClassName,      //利用するクラス名
		titleName.c_str(),		//タイトルバーの文字
		style_,					//よく見るウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標
		CW_USEDEFAULT,			//表示Y座標
		wrc_.right - wrc_.left,	//ウィンドウ横幅
		wrc_.bottom - wrc_.top,	//ウィンドウ縦幅
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		wc_.hInstance,			//インスタンスハンドル
		nullptr					//オプション
	);
}

void Window::Initialize(const std::wstring& windowClassName, const std::wstring& titleName)
{
	SetUpClass(windowClassName);
	SetUpWindow();
	MakeWindow(titleName);
	Show();
}

#pragma region ウィンドウプロシージャ(LRESULT CALLBACK WindowProc)
LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg,
	WPARAM wparam, LPARAM lparam) {
	// たぶんImGuiのところで使うからImGuiクラスを作った後
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
#pragma endregion

void Window::SetCursorShape() {
	// カーソル
	wc_.hCursor = LoadCursor(nullptr, cursorType_);
}