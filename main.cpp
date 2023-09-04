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

	//��ʼ����͸������
	WNDCLASSEX wc{ };
	wc.cbSize = sizeof(wc);						//���ڵĴ�С,һ�㶼��sizeof��ȡ��׼��С
	wc.style = CS_HREDRAW | CS_VREDRAW;			//ˮƽ�ʹ�ֱ�ػ�
	wc.lpfnWndProc = (WNDPROC)OverWndMsgProc;	//������Ϣ�ص����� ������¼���ڴ����Ͻ��е�һЩ����
	wc.hInstance = GetModuleHandle(NULL);		//����ʵ��������������κ���
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);   //���ڹ���� ��ʽ
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);	//���ڱ�����ɫ ��ɫ
	wc.lpszClassName = "OverlayWindow";			//�����������
	//ע�ᴰ����
	RegisterClassEx(&wc);
	//��������
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

	//���ô���͸����
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	//��ʾ����
	UpdateWindow(hwnd);
	ShowWindow(hwnd, SW_SHOW);

	//����
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

	//��׼
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
		//ȷ������ʼ������ǰ��
		GetWindowRect(hgame, &game_rect);
		SetWindowPos(hwnd, HWND_TOPMOST, game_rect.left, game_rect.top, game_rect.right - game_rect.left, game_rect.bottom - game_rect.top, SWP_SHOWWINDOW);
		//�����ɰ洰����Ϣ
		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
			//�����������Ϣת��Ϊ�ַ���Ϣ
			TranslateMessage(&msg);
			//����Ϣ�̸���Ϣ����
			DispatchMessage(&msg);
		}
	}
	//UnhookWindowsHookEx(hMouseHook);
	return 0;
}