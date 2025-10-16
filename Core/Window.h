#pragma once
#include <Windows.h>
#include <string>




class Window
{
public:
	
	void Initialize(const std::wstring& windowClassName, const std::wstring& titleName);
	
	void SetCursorType(const LPCWSTR& cursorType) { cursorType_ = cursorType; }

	void SetWindowShowMode(int mode) { windowShowMode_ = mode; }

	HWND GetHwnd() const { return hwnd_; }

	WNDCLASS& GetWindowClass(){ return wc_; }

	RECT GetWindowRect() const { return wrc_; }

	int32_t GetHeight()const { return kClientHeight_; }

	int32_t GetWidth()const { return kClientWidth_; }

private:
/// <summary>
/// ウィンドウプロシージャ
/// </summary>
/// <param name="hwnd ウィンドウの識別番号"></param>
/// <param name="msg なにが起きたかを渡す"></param>
/// <param name="wparam 押されたキーの情報など"></param>
/// <param name="lparam カーソルの座標"></param>
/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// 今動いているプログラムの情報を獲得
	/// </summary>
	void SetHInstance() { wc_.hInstance = GetModuleHandle(nullptr); }

	/// <summary>
	/// カーソルの形状を設定
	/// </summary>
	void SetCursorShape();

	/// <summary>
	/// wrc = { 左端, 上, 横幅, 高さ}
	/// </summary>
	void SetUpSize() {wrc_ = { kClientLeft_, kClientTop_, kClientWidth_, kClientHeight_ };}

	/// <summary>
	/// 本当のウィンドウのサイズを設定
	/// </summary>
	void SetUpWindow();

	/// <summary>
	/// ウィンドウクラスの設定
	/// </summary>
	/// <param name="windowClassName クラスの名前"></param>
	void SetUpClass(const std::wstring& windowClassName);

	/// <summary>
	/// ウィンドウの生成
	/// </summary>
	/// <param name="titleName タイトルバーの名前"></param>
	void MakeWindow(const std::wstring& titleName);

	/// <summary>
	/// ウィンドウの表示
	/// </summary>
	void Show() { ShowWindow(hwnd_, windowShowMode_); }

private:
	int32_t kClientLeft_ = 0; // クライアント領域の左端
	int32_t kClientTop_ = 0; // クライアント領域の上端
	int32_t kClientWidth_ = 1280; // クライアント領域の幅
	int32_t kClientHeight_ = 720; // クライアント領域の高さ

	WNDCLASS wc_;
	RECT wrc_;// window rect 
	LPCWSTR cursorType_; // カーソルの種類
	DWORD style_ = WS_OVERLAPPEDWINDOW; // ウィンドウのスタイル

	HWND hwnd_;
	int windowShowMode_ = SW_SHOW;
};

