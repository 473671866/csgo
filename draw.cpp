#include "draw.h"

boolean GetClientRectEx(HWND hwnd, int32_t& width, int32_t& height)
{
	RECT rect{};
	boolean success = GetClientRect(hwnd, &rect);
	if (success) {
		width = rect.right - rect.left;   //�ڴ��ڷֱ��ʿ��
		height = rect.bottom - rect.top;   //�ڴ��ڷֱ��ʸ߶�
	}
	return success;
}

boolean GetWindowRectEx(HWND hwnd, int32_t& width, int32_t& height)
{
	RECT rect{};
	boolean success = GetWindowRect(hwnd, &rect);
	if (success) {
		width = rect.right - rect.left;   //�ⴰ�ڷֱ��ʿ��
		height = rect.bottom - rect.top;   //�ⴰ�ڷֱ��ʸ߶�
	}
	return success;
}

boolean WordToScrenn(int32_t width, int32_t height, WorldCoor& worldcoor, ScreenCoor& screencoor, float_t matrix[4][4])
{
	ClipCoor clipcoor;
	clipcoor.x = matrix[0][0] * worldcoor.x + matrix[0][1] * worldcoor.y + matrix[0][2] * worldcoor.z + matrix[0][3];
	clipcoor.y = matrix[1][0] * worldcoor.x + matrix[1][1] * worldcoor.y + matrix[1][2] * worldcoor.z + matrix[1][3];
	clipcoor.z = matrix[2][0] * worldcoor.x + matrix[2][1] * worldcoor.y + matrix[2][2] * worldcoor.z + matrix[2][3];
	clipcoor.w = matrix[3][0] * worldcoor.x + matrix[3][1] * worldcoor.y + matrix[3][2] * worldcoor.z + matrix[3][3];

	//�ж�������ﲻ��������Ұ����
	if (clipcoor.w < 0.01) {
		return false;
	};

	//�������� ---> NDC����
	NdcCoor NDC;
	NDC.x = clipcoor.x / clipcoor.w;
	NDC.y = clipcoor.y / clipcoor.w;
	NDC.z = clipcoor.z / clipcoor.w;

	//NDC���� ---> ��Ļ����
	screencoor.x = width / 2 + (width / 2) * NDC.x;
	screencoor.y = height / 2 - (height / 2) * NDC.y;

	return true;
}

inline boolean DrawRect(HDC hdc, HBRUSH hbrush, int32_t x, int32_t y, int32_t w, int32_t t)
{
	RECT rect = { x, y, x + w, y + t };
	return FillRect(hdc, &rect, hbrush);
}

boolean DrawBox(HDC hdc, HBRUSH hbrush, int32_t x, int32_t y, int32_t w, int32_t h, int32_t t)
{
	//����
	DrawRect(hdc, hbrush, x, y, w, t);
	//���
	DrawRect(hdc, hbrush, x, y + t, t, h - t);
	//�ұ�
	DrawRect(hdc, hbrush, (x + w) - t, y + t, t, h - t);
	//�ױ�
	return DrawRect(hdc, hbrush, x + t, y + h - t, w - t * 2, t);
}

boolean DrawLine(HDC hdc, int32_t width, int32_t height, int32_t x, int32_t y)
{
	MoveToEx(hdc, width / 2, height, NULL);
	return LineTo(hdc, x, y);
}