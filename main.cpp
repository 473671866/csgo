#include <iostream>
#include <thread>
#include <windows.h>
#include "business.h"

LRESULT OverWndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	case WM_CLOSE: {
		DestroyWindow(hWnd);
		break;
	}
	default: {
		break;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		switch (wParam) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN: {
			AutoAim();
		}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main()
{
	HWND hgame = GetGameWindow();
	RECT game_rect{};
	GetWindowRect(hgame, &game_rect); game_rect.top += 25;

	//开始创建透明窗口
	WNDCLASSEX wc{ };
	wc.cbSize = sizeof(wc);						//窗口的大小,一般都用sizeof获取标准大小
	wc.style = CS_HREDRAW | CS_VREDRAW;			//水平和垂直重绘
	wc.lpfnWndProc = (WNDPROC)OverWndMsgProc;	//窗口消息回调函数 用来记录你在窗户上进行的一些操作
	wc.hInstance = GetModuleHandle(NULL);		//窗口实例句柄，不依赖任何项
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);   //窗口光标句柄 样式
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);	//窗口背景颜色 白色
	wc.lpszClassName = "OverlayWindow";			//窗口类的名称
	//注册窗口类
	RegisterClassEx(&wc);
	//创建窗口
	HWND hwnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TRANSPARENT,
		wc.lpszClassName, "over",
		WS_POPUP,
		game_rect.left,
		game_rect.top,
		game_rect.right - game_rect.left,
		game_rect.bottom - game_rect.top,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	//设置窗口透明度
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	//显示窗口
	UpdateWindow(hwnd);
	ShowWindow(hwnd, SW_SHOW);

	//绘制
	std::thread draw_thread([hwnd] {
		while (true) {
			HWND hgame = GetGameWindow();
			boolean success = Flash(hwnd, hgame);
			if (success) {
				Paint(hwnd);
			}
			Sleep(30);
		}
		});
	draw_thread.detach();

	//瞄准
	std::thread aim_thread([] {
		while (true) {
			SHORT state = GetAsyncKeyState(VK_LBUTTON);
			if (state & 0x8000) {
				AutoAim();
			}
			state = GetAsyncKeyState(VK_RBUTTON);
			if (state & 0x8000) {
				AutoAim();
			}
		}
		});
	aim_thread.detach();

	//HHOOK hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);

	MSG msg{};
	while (msg.message != WM_QUIT) {
		//确保窗口始终在最前面
		GetWindowRect(hgame, &game_rect);
		SetWindowPos(hwnd, HWND_TOPMOST, game_rect.left, game_rect.top, game_rect.right - game_rect.left, game_rect.bottom - game_rect.top, SWP_SHOWWINDOW);
		//处理蒙版窗口消息
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
			//把虚拟键盘消息转化为字符消息
			TranslateMessage(&msg);
			//把消息教给消息窗体
			DispatchMessage(&msg);
		}
	}
	//UnhookWindowsHookEx(hMouseHook);
	return 0;
}