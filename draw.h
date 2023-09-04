#pragma once
#include <iostream>
#include <windows.h>

//世界坐标
struct WorldCoor
{
	float_t x;
	float_t y;
	float_t z;
};

//剪辑坐标
struct ClipCoor
{
	float x, y, z, w;
};

//ndc坐标
struct NdcCoor
{
	float_t x;
	float_t y;
	float_t z;
};

//屏幕坐标
struct ScreenCoor
{
	float_t x;
	float_t y;
};

boolean GetClientRectEx(HWND hwnd, int32_t& width, int32_t& height);
boolean GetWindowRectEx(HWND hwnd, int32_t& width, int32_t& height);
boolean WordToScrenn(int32_t width, int32_t height, WorldCoor& worldcoor, ScreenCoor& screencoor, float_t matrix[4][4]);
boolean DrawBox(HDC hdc, HBRUSH hbrush, int32_t x, int32_t y, int32_t w, int32_t h, int32_t t);
boolean DrawLine(HDC hdc, int32_t width, int32_t height, int32_t x, int32_t y);