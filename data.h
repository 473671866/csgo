#pragma once
#include <iostream>
#include <windows.h>
#include <vector>

struct Player
{
	uint32_t hp;
	uint32_t camp;
	float_t x;
	float_t y;
	float_t z;
	float_t bx;
	float_t by;
	float_t bz;
};

boolean GetPlayer(Player& player);
boolean GetPalyerList(std::vector<Player>* collection);
boolean GetMatrix(float_t matrix[4][4]);
boolean ModifyToward(float_t horizontal, float_t vertical);
boolean GetToward(float_t* horizontal, float_t* vertical);