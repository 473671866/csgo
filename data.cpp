#include "data.h"
#include "helper.h"

#define PLAYER_LIST 0x4DFFF7C
#define PLAYER_CAMP 0xf4
#define PLAYER_HP 0x100
#define PLAYER_X 0xa0
#define PLAYER_Y 0xa4
#define PLAYER_Z 0xa8

#define PLAYER_BONES 0x26a8
#define PALYER_BONES_X 0x18C
#define PALYER_BONES_y 0x19C
#define PALYER_BONES_Z 0x1aC

#define MATRIX 0x4DF0DC4

#define TOWARD 0x59F19C
#define VERTICAL 0x4D90
#define HORIZONTAL 0x4D94

boolean GetPlayer(Player& player)
{
	static Helper* helper = Helper::Make();
	static uint_ptr module_address = helper->GetModuleAddress("client.dll");

	uint32_t base = 0;
	uint32_t head = module_address + PLAYER_LIST;
	helper->ReadMemory(head, &base, sizeof(base));

	if (base != 0) {
		helper->ReadMemory(base + PLAYER_CAMP, &player.camp, sizeof(player.camp));
		helper->ReadMemory(base + PLAYER_HP, &player.hp, sizeof(player.hp));
		helper->ReadMemory(base + PLAYER_X, &player.x, sizeof(player.x));
		helper->ReadMemory(base + PLAYER_Y, &player.y, sizeof(player.y));
		helper->ReadMemory(base + PLAYER_Z, &player.z, sizeof(player.z));

		uint32_t bones = 0;
		helper->ReadMemory(base + PLAYER_BONES, &bones, sizeof(bones));
		helper->ReadMemory(bones + PALYER_BONES_X, &player.bx, sizeof(player.bx));
		helper->ReadMemory(bones + PALYER_BONES_y, &player.by, sizeof(player.by));
		helper->ReadMemory(bones + PALYER_BONES_Z, &player.bz, sizeof(player.bz));
		return true;
	}

	return false;
}

boolean GetPalyerList(std::vector<Player>* collection)
{
	static Helper* helper = Helper::Make();
	static uint_ptr module_address = helper->GetModuleAddress("client.dll");

	uint32_t base = 0;
	uint32_t head = module_address + PLAYER_LIST;
	helper->ReadMemory(head, &base, sizeof(base));

	uint32_t team = 0;
	helper->ReadMemory(base + PLAYER_CAMP, &team, sizeof(team));

	for (int i = 0; i <= 32; i++) {
		Player player{};

		helper->ReadMemory(head += 0x10, &base, sizeof(base));
		if (base == 0) {
			continue;
		}

		helper->ReadMemory(base + PLAYER_CAMP, &player.camp, sizeof(player.camp));
		//if (team == player.camp) {
		//	continue;
		//}

		helper->ReadMemory(base + PLAYER_HP, &player.hp, sizeof(player.hp));
		if (player.hp <= 0) {
			continue;
		}

		helper->ReadMemory(base + PLAYER_X, &player.x, sizeof(player.x));
		helper->ReadMemory(base + PLAYER_Y, &player.y, sizeof(player.y));
		helper->ReadMemory(base + PLAYER_Z, &player.z, sizeof(player.z));

		uint32_t bones = 0;
		helper->ReadMemory(base + PLAYER_BONES, &bones, sizeof(bones));
		helper->ReadMemory(bones + PALYER_BONES_X, &player.bx, sizeof(player.bx));
		helper->ReadMemory(bones + PALYER_BONES_y, &player.by, sizeof(player.by));
		helper->ReadMemory(bones + PALYER_BONES_Z, &player.bz, sizeof(player.bz));

		collection->push_back(player);
	}

	return collection->size() > 0;
}

boolean GetMatrix(float_t matrix[4][4])
{
	static Helper* helper = Helper::Make();
	static uint_ptr module_address = helper->GetModuleAddress("client.dll");
	return helper->ReadMemory(module_address + MATRIX, matrix, 64);
}

boolean ModifyToward(float_t horizontal, float_t vertical)
{
	static Helper* helper = Helper::Make();
	static uint_ptr module_address = helper->GetModuleAddress("engine.dll");
	uint32_t base = 0;
	helper->ReadMemory(module_address + TOWARD, &base, sizeof(base));
	if (base != 0) {
		helper->WriteMemory(base + HORIZONTAL, &horizontal, sizeof(horizontal));
		helper->WriteMemory(base + VERTICAL, &vertical, sizeof(vertical));
		return true;
	}
	return false;
}

boolean GetToward(float_t* horizontal, float_t* vertical)
{
	static Helper* helper = Helper::Make();
	static uint_ptr module_address = helper->GetModuleAddress("engine.dll");
	uint32_t base = 0;
	helper->ReadMemory(module_address + TOWARD, &base, sizeof(base));
	if (base != 0) {
		helper->ReadMemory(base + HORIZONTAL, horizontal, sizeof(horizontal));
		helper->ReadMemory(base + VERTICAL, vertical, sizeof(vertical));
		return true;
	}
	return false;
}