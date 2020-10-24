#pragma once

#include "D2Structs.h"

void __declspec(naked) __fastcall D2CLIENT_ClickItemRight_ASM(DWORD x, DWORD y, DWORD Location, DWORD Player, DWORD pUnitInventory) {
	__asm
	{
		xchg edx, ecx // x, y -> y, x
		pop eax // pop return address
		xchg eax, [esp] // return address to stack, location to eax
		jmp D2CLIENT_ClickItemRight_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_SetSelectedUnit_STUB(DWORD UnitAny) {
	__asm
	{
		mov eax, ecx
		jmp D2CLIENT_SetSelectedUnit_I
	}
}

UnitAny* GetCubeUnit(UnitAny* player) {
	for (UnitAny* pItem = D2COMMON_GetItemFromInventory(player->pInventory); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem)) {
		if (pItem->dwTxtFileNo == 549) {
			return pItem;
		}
  }
	return NULL;
}

UnitAny* GetInventoryItemByTxtFileNo(UnitAny* player, int txtFileNo) {
	for (UnitAny* pItem = D2COMMON_GetItemFromInventory(player->pInventory); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem)) {
		if (pItem->dwTxtFileNo == txtFileNo) {
			return pItem;
		}
  }
	return NULL;
}
