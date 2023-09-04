#include "business.h"
#include <math.h>
#include <limits>

#define MINIMUN 15

HWND GetGameWindow()
{
	static HWND hwnd = FindWindowA("Valve001", NULL);
	return hwnd;
}

boolean Flash(HWND hwnd, HWND hgame)
{
	//�ж�CSGO�����Ƿ�����ǰ��
	HWND foreground = GetForegroundWindow();
	if (foreground == hwnd) {
		return false;
	}

	//ʵ��͸���ɰ洰�ڵ�ˢ��
	RECT game_rect{};
	GetWindowRect(hgame, &game_rect);
	RECT overlay_rect{ 0,0, game_rect.right - game_rect.left, game_rect.bottom - game_rect.top };

	HDC hdc = GetDC(hwnd);
	HBRUSH hbrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	FillRect(hdc, &overlay_rect, hbrush);
	DeleteObject(hbrush);
	ReleaseDC(hwnd, hdc);
	return MoveWindow(hwnd, game_rect.left, game_rect.top, game_rect.right - game_rect.left, game_rect.bottom - game_rect.top, TRUE);
}

boolean Paint(HWND hwnd)
{
	int32_t width = 0;
	int32_t height = 0;
	GetWindowRectEx(hwnd, width, height);

	std::vector<Player>collection;
	GetPalyerList(&collection);

	float_t matrix[4][4]{};
	GetMatrix(matrix);

	HDC hdc = GetDC(hwnd);
	HBRUSH hbrush = CreateSolidBrush(RGB(128, 0, 0));

	HPEN hpen = CreatePen(PS_SOLID, 2, RGB(128, 0, 0));
	SelectObject(hdc, hpen);

#pragma warning(push)
#pragma warning(disable: 4244)
	for (Player& player : collection) {
		//����
		ScreenCoor bonessrceen{};
		WorldCoor bonesworld{ .x = player.bx, .y = player.by, .z = player.bz };
		boolean success = WordToScrenn(width, height, bonesworld, bonessrceen, matrix);
		if (!success) {
			continue;
		}

		//��ɫ
		ScreenCoor playersrceen{};
		WorldCoor playerworld{ .x = player.x, .y = player.y, .z = player.z };
		success = WordToScrenn(width, height, playerworld, playersrceen, matrix);
		if (!success) {
			continue;
		}

		//����
		float_t box_height = playersrceen.y - bonessrceen.y;
		float_t box_width = box_height / 2;
		DrawBox(hdc, hbrush, bonessrceen.x - box_width / 2, bonessrceen.y - 15, box_width, box_height + 15, 1);

		//������
		DrawLine(hdc, width, height, playersrceen.x, playersrceen.y);
	}
#pragma warning(pop)

	DeleteObject(hpen);
	DeleteObject(hbrush);
	ReleaseDC(hwnd, hdc);
	return true;
}

boolean AutoAim()
{
	std::vector<Player>collection;
	GetPalyerList(&collection);

	Player self{};
	GetPlayer(self);

	float_t currnent_horiziontal = 0;//��ǰˮƽ����
	float_t current_vertical = 0;//��ǰ��ֱ����
	float_t horizontal = 0;//ˮƽ����
	float_t vertical = 0;//��ֱ����
	float_t final_vertical = 0;//��ֱ���������
	float_t final_horizontalcal = 0;//��ֱ���������
	float_t difference_horizontal = 0;//ˮƽ����ǶȲ�
	float_t difference_vertical = 0;//��ֱ����ǶȲ�
	float_t angle_and = 0;//����ǶȺ�
	float_t minimum_angle_and = MINIMUN; //��С�ǶȺ�
	float_t distance = 0;//ƽ��ֱ�߾���
	float_t y = 0;
	float_t x = 0;
	float_t z = 0;

	for (Player& player : collection) {
		//ˮƽ����
		//1
		if (player.bx >= self.bx && player.by >= self.by) {
			y = player.by - self.by;
			x = player.bx - self.bx;
			horizontal = static_cast<float_t>(static_cast<double_t>(atan2(y, x) * 180 / 3.1415));
		}
		//2
		else if (player.bx <= self.bx && player.by >= self.by) {
			y = player.by - self.by;
			x = self.bx - player.bx;
			horizontal = 180 - static_cast<float_t>(static_cast<double_t>(atan2(y, x) * 180 / 3.1415));
		}
		//3
		else if (player.bx <= self.bx && player.by <= self.by) {
			y = self.by - player.by;
			x = self.bx - player.bx;
			horizontal = -180 + static_cast<float_t>(static_cast<double_t>(atan2(y, x) * 180 / 3.1415));
		}
		//4
		else if (player.bx >= self.bx && player.by <= self.by) {
			y = self.by - player.by;
			x = player.bx - self.bx;
			horizontal = -static_cast<float_t>(static_cast<double_t>(atan2(y, x) * 180 / 3.1415));
		}

		//����
		distance = sqrt((player.by - self.by) * (player.by - self.by) + (player.bx - self.bx) * (player.bx - self.bx));
		//��ֱ����
		//��
		if (player.bz >= self.bz) {
			z = player.bz - self.bz;
			vertical = -static_cast<float_t>(static_cast<double_t>(atan2(z, distance) * 180 / 3.1415));
		}
		//��
		if (player.bz <= self.bz) {
			z = self.bz - player.bz;
			vertical = static_cast<float_t>(static_cast<double_t>(atan2(z, distance) * 180 / 3.1415));
		}

		//����ǶȲ�
		GetToward(&currnent_horiziontal, &current_vertical);
		//��ֱ
		difference_vertical = vertical - current_vertical;
		//ˮƽ
		difference_horizontal = currnent_horiziontal - horizontal;
		if (difference_horizontal <= -180) {
			difference_horizontal += 360;
		}
		else if (difference_horizontal >= 180) {
			difference_horizontal -= 360;
		}

		angle_and = abs(difference_horizontal) + fabs(difference_vertical);
		if (angle_and < MINIMUN) {
			if (angle_and <= minimum_angle_and) {
				minimum_angle_and = angle_and;
				final_vertical = vertical;
				final_horizontalcal = horizontal;
			}
		}
	}

	if (final_vertical != 0 && final_horizontalcal != 0) {
		return ModifyToward(final_horizontalcal, final_vertical);
	}

	return false;
}