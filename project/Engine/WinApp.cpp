#include "WinApp.h"
#include <Windows.h>

bool WinApp::ProcessMessage() {
	MSG msg{};
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}