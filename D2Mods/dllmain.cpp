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
	}
	return (LONG)CallWindowProcA(origWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(0, 0, ATThread, 0, 0, 0);
        origWndProc = (WNDPROC)GetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC);
        SetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)GameWindowEvent);
        Patch(Jump, 0x5ADB3, (int)GameAutomapDraw_Interception, 5).Install();
    } else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
    }
    return TRUE;
}
