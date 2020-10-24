#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Maphack.h"
#include "Drawing.h"
#include "CursorGlitch.h"

bool reveal = false;

DWORD WINAPI ATThread(VOID* lpvoid) {
    while (true) {
        if (reveal) {
            Maphack().RevealGame();
            reveal = false;
        }
    }
    return 1;
}

WNDPROC origWndProc;

LONG WINAPI GameWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_KEYDOWN) {
        if (wParam == VK_F5)
        {
            auto player = D2CLIENT_GetPlayerUnit();
            if (player) reveal = true;
        }
        //if (wParam == VK_F6) {
        //  auto player = D2CLIENT_GetPlayerUnit();
        //  if (player) {
        //    CursorGlitchFix cfg = CursorGlitchFix();

        //    *p_D2CLIENT_CursorItemMode = ITEM_MODE_ON_GROUND;
        //    player->pInventory->pCursorItem = NULL;
        //    cfg.DisableBusyState();
        //    cfg.OpenCube(player);

        //    UnitAny* swordguard = GetInventoryItemByTxtFileNo(player, 38);
        //    if (swordguard) {
        //      cfg.DisplayItemInfo(swordguard);
        //      cfg.MoveItemToCursorBuffer(swordguard->dwUnitId);
        //      cfg.DropCursorItem(swordguard->dwUnitId);
        //    }

        //    if (player->pInventory->pCursorItem) {
        //      //player->pInventory->pCursorItem->dwType = (DWORD)(*p_D2CLIENT_RegularCursorType);
        //      cfg.DisplayItemInfo(player->pInventory->pCursorItem);
        //      //D2CLIENT_SetSelectedUnit(NULL);
        //      cfg.MoveItemToCursorBuffer(player->pInventory->pCursorItem->dwUnitId);
        //      //cfg.MoveItemToCursorBuffer(player->pInventory->pCursorItem->dwTxtFileNo);
        //      //Sleep(100);
        //      //cfg.DisplayItemInfo(player->pInventory->pCursorItem);
        //      //cfg.DropCursorItem(player->pInventory->pCursorItem->dwTxtFileNo);
        //      cfg.DropCursorItem(player->pInventory->pCursorItem->dwUnitId);
        //      //player->pInventory->pCursorItem->dwType = *p_D2CLIENT_RegularCursorType;
        //      //player->pInventory->pCursorItem->dwMode = ITEM_MODE_BEING_DROPPED;
        //      //D2CLIENT_CloseInteract();
        //      //D2CLIENT_ClickMap(0, 10, 10, 0x08);
        //      //player->pInventory->pCursorItem = nullptr;
        //    }
        //  }
        //}
	}
	return (LONG)CallWindowProcA(origWndProc, hWnd, uMsg, wParam, lParam);
}

void Hooks() {
    origWndProc = (WNDPROC)GetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC);
    SetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)GameWindowEvent);
    Patch(Jump, 0x5ADB3, (int)GameAutomapDraw_Interception, 5).Install();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(0, 0, ATThread, 0, 0, 0);
        Hooks();
    } else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
    }
    return TRUE;
}

