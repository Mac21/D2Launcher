#pragma once

#include "D2Constants.h"
#include "D2Structs.h"
#include "D2Offsets.h"


class CursorGlitchFix {
public:
	void DisplayItemInfo(UnitAny* item) {
    PrintText(1, "ÿc4CursorGlitch:ÿc1 Item txt file no: %u (%x).", item->dwTxtFileNo, item->dwTxtFileNo);
    PrintText(1, "ÿc4CursorGlitch:ÿc1 Item Unit ID: %u (%x).", item->dwUnitId, item->dwUnitId);
		if (item->pItemData) {
			PrintText(1, "ÿc4CursorGlitch:ÿc1 Item location: %u (%x).", item->pItemData->ItemLocation, item->pItemData->ItemLocation);
			PrintText(1, "ÿc4CursorGlitch:ÿc1 Node location: %u (%x).", item->pItemData->NodePage, item->pItemData->NodePage);
			PrintText(1, "ÿc4CursorGlitch:ÿc1 Game location: %u (%x).", item->pItemData->GameLocation, item->pItemData->GameLocation);
		}
	}

	void OpenCube(UnitAny* player) {
		UnitAny* cube = GetCubeUnit(player);
		if (cube == NULL) {
			return;
		}

		InventoryLayout* pLayout = (InventoryLayout*)p_D2CLIENT_InventoryLayout;
		int ClickLocation = CLICKTARGET_INVENTORY;

		*p_D2CLIENT_CursorHoverX = cube->pItemPath->dwPosX;
		*p_D2CLIENT_CursorHoverY = cube->pItemPath->dwPosY;

		int x = pLayout->Left + cube->pItemPath->dwPosX * pLayout->SlotPixelWidth + 10;
		int y = pLayout->Top + cube->pItemPath->dwPosY * pLayout->SlotPixelHeight + 10;

		D2CLIENT_RightClickItem(x, y, ClickLocation, D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);

		BYTE aPacket[13] = { NULL };
		aPacket[0] = 0x20;
		*(DWORD*)&aPacket[1] = cube->dwUnitId;
		*(DWORD*)&aPacket[5] = (DWORD)x;
		*(DWORD*)&aPacket[9] = (DWORD)y;
		D2NET_SendPacket(13, 1, aPacket);
	}

	void MoveItemToBuffer(DWORD itemID) {
		LPBYTE aPacket = new BYTE[17];
		aPacket[0] = (BYTE)0x18;
		*(DWORD32*)&aPacket[1] = itemID;
		D2NET_SendPacket(17, 1, aPacket);
		delete[] aPacket;
	}

	bool DropCursorItem(DWORD itemID) {
		LPBYTE aPacket = new BYTE[5];
		aPacket[0] = (BYTE)0x17;
		*(DWORD*)&aPacket[1] = (DWORD)itemID;
		D2NET_SendPacket(5, 1, aPacket);
		delete[] aPacket;
		return 1;
	}

	bool Interact(DWORD UnitId, DWORD UnitType) {
		LPBYTE aPacket = new BYTE[9];
		aPacket[0] = (BYTE)0x13;
		*(DWORD*)&aPacket[1] = UnitType;
		*(DWORD*)&aPacket[5] = UnitId;
		D2NET_SendPacket(9, 1, aPacket);
		delete[] aPacket;
		return 1;
	}

	bool DisableBusyState() {
		LPBYTE aPacket = new BYTE[1];
		aPacket[0] = (BYTE)0x48;
		D2NET_SendPacket(1, 1, aPacket);
		delete[] aPacket;
		return 1;
	}

	bool MoveItemToCursorBuffer(DWORD itemID) {
		LPBYTE aPacket = new BYTE[5];
		aPacket[0] = (BYTE)0x19;
		*(DWORD*)&aPacket[1] = itemID;
		D2NET_SendPacket(5, 1, aPacket);
		delete[] aPacket;
		return 1;
	}

	void RefreshWindow() {
		LPBYTE aPacket = new BYTE[5];
		aPacket[0] = (BYTE)0x4F;
		*(DWORD*)&aPacket[1] = (DWORD)0x08;
		*(WORD*)&aPacket[5] = (WORD)0;
		D2NET_SendPacket(5, 1, aPacket);
		delete[] aPacket;
	}
};
