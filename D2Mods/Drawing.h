#pragma once
#include <windows.h>
#include <string>
#include <algorithm>
#include <array>

#include "Patch.h"
#include "D2Constants.h"
#include "D2Offsets.h"

// https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
#define CP_WINDOWS_ANSI_LATIN 1252

wchar_t* AnsiToUnicode(const char* str, UINT codepage = CP_UTF8) {
	wchar_t* buf = NULL;
	int len = MultiByteToWideChar(codepage, 0, str, -1, buf, 0);
	buf = new wchar_t[len];
	MultiByteToWideChar(codepage, 0, str, -1, buf, len);
	return buf;
}

char* UnicodeToAnsi(const wchar_t* str) {
	char* buf = NULL;
	int len = WideCharToMultiByte(CP_WINDOWS_ANSI_LATIN, 0, str, -1, buf, 0, "?", NULL);
	buf = new char[len];
	WideCharToMultiByte(CP_WINDOWS_ANSI_LATIN, 0, str, -1, buf, len, "?", NULL);
	return buf;
}

std::string GetItemCode(int dwTxtFileNo) {
	ItemTxt* text = D2COMMON_GetItemText(dwTxtFileNo);
	std::string code = text->szCode;
	return code.substr(0, 3);
}

std::string GetItemName(UnitAny* item) {
	wchar_t buffer[256] = L"";
	D2CLIENT_GetItemName(item, buffer, 256);
	return UnicodeToAnsi(buffer);
}

POINT CalculateTextLen(const wchar_t* szwText, int Font) {
	POINT ret = { 0, 0 };
	if (!szwText) return ret;
	DWORD dwWidth, dwFileNo;
	DWORD dwOldSize = D2WIN_SetTextSize(Font);
	ret.y = D2WIN_GetTextSize((wchar_t*)szwText, &dwWidth, &dwFileNo);
	ret.x = dwWidth;
	D2WIN_SetTextSize(dwOldSize);
	return ret;
}

POINT CalculateTextLen(const char* szwText, int Font) {
	POINT ret = { 0, 0 };
	if (!szwText) return ret;
	wchar_t* buf = AnsiToUnicode(szwText);
	ret = CalculateTextLen(buf, Font);
	delete[] buf;
	return ret;
}

bool IsInGame(void) {
	UnitAny* player = D2CLIENT_GetPlayerUnit();
	Control* firstControl = *p_D2WIN_FirstControl;
	if (player && !firstControl) {
		if (player->pInventory && player->pPath && player->pPath->pRoom1 && player->pPath->pRoom1->pRoom2 && player->pPath->pRoom1->pRoom2->pLevel && player->pPath->pRoom1->pRoom2->pLevel->dwLevelNo)
			return true;
	}
	return false;
}

POINT ScreenToAutomap(int x, int y) {
	POINT result = { 0, 0 };
	x *= 32;
	y *= 32;
	result.x = ((x - y) / 2 / (*p_D2CLIENT_Divisor)) - (*p_D2CLIENT_Offset).x + 8;
	result.y = ((x + y) / 4 / (*p_D2CLIENT_Divisor)) - (*p_D2CLIENT_Offset).y - 8;

	if (D2CLIENT_GetAutomapSize()) {
		--result.x;
		result.y += 5;
	}
	return result;
}

POINT GetScreenSize() {
	POINT ingame = { *p_D2CLIENT_ScreenSizeX, *p_D2CLIENT_ScreenSizeY }, oog = { 800, 600 };
	return IsInGame() ? ingame : oog;
}

POINT GetTextSize(std::string text, unsigned int font) {
	unsigned int height[] = { 10,11,18,24,10,13,7,13,10,12,8,8,7,12 };
	DWORD width, fileNo;
	wchar_t* wString = AnsiToUnicode(text.c_str());
	DWORD oldFont = D2WIN_SetTextSize(font);
	D2WIN_GetTextWidthFileNo(wString, &width, &fileNo);
	D2WIN_SetTextSize(oldFont);
	POINT point = { width, height[font] };
	delete[] wString;
	return point;
}

bool DrawString(unsigned int x, unsigned int y, int align, unsigned int font, TextColor color, std::string text, ...) {
	char buffer[4096];
	va_list arg;
	va_start(arg, text);
	vsprintf_s(buffer, 4096, text.c_str(), arg);
	va_end(arg);
	wchar_t* wString = AnsiToUnicode(buffer);
	unsigned int properX = x;
	if (align == 1) x = x - (GetTextSize(buffer, font).x / 2);
	if (align == 2) x = x - GetTextSize(buffer, font).x;
	unsigned int height[] = { 10,11,18,24,10,13,7,13,10,12,8,8,7,12 };
	if (align == 3) y = y + height[font];
	DWORD size = D2WIN_SetTextSize(font);
	D2WIN_DrawText(wString, x, y, color, 0);
	D2WIN_SetTextSize(size);
	delete[] wString;
	return true;
}

bool DrawBox(unsigned int x, unsigned int y, unsigned int xSize, unsigned int ySize, unsigned int color, BoxTrans trans) {
	D2GFX_DrawRectangle(x, y, x + xSize, y + ySize, color, trans);
	return true;
}

bool DrawCrosshook(unsigned int x, unsigned int y, unsigned int color) {
	CHAR szLines[][2] = { 0,-2, 4,-4, 8,-2, 4,0, 8,2, 4,4, 0,2, -4,4, -8,2, -4,0, -8,-2, -4,-4, 0,-2 };
	for (unsigned int n = 0; n < 12; n++) {
		D2GFX_DrawLine(x + szLines[n][0], y + szLines[n][1], x + szLines[n + 1][0], y + szLines[n + 1][1], color, -1);
	}
	return true;
}

bool IsValidMonster(UnitAny* pUnit)
{
	if (!pUnit) return false;
	if ((pUnit->dwMode == 0) || (pUnit->dwMode == 12)) return false;
	if ((pUnit->dwTxtFileNo == 608) && (pUnit->dwMode == 8)) return false;
	if ((pUnit->dwTxtFileNo == 68) && (pUnit->dwMode == 14)) return false;
	if ((pUnit->dwTxtFileNo == 258 || (pUnit->dwTxtFileNo == 261)) && (pUnit->dwMode == 14)) return false;
	DWORD badMonIds[] = { 227, 283, 326, 327, 328, 329, 330, 410, 411, 412, 413, 414, 415, 416, 366, 406, 351, 352, 353, 266, 408, 516, 517, 518, 519, 522, 523, 543, 543, 545 };
	for (DWORD n = 0; n < 30; n++) if (pUnit->dwTxtFileNo == badMonIds[n]) return false;
	if (D2COMMON_GetUnitStat(pUnit, 172, 0) == 2) return false;
	return true;
}

bool IsObjectChest(ObjectTxt* obj)
{
	return (obj->nSelectable0 && (
		(obj->nOperateFn == 1) || //bed, undef grave, casket, sarc
		(obj->nOperateFn == 3) || //basket, urn, rockpile, trapped soul
		(obj->nOperateFn == 4) || //chest, corpse, wooden chest, buriel chest, skull and rocks, dead barb
		(obj->nOperateFn == 5) || //barrel
		(obj->nOperateFn == 7) || //exploding barrel
		(obj->nOperateFn == 14) || //loose bolder etc....*
		(obj->nOperateFn == 19) || //armor stand
		(obj->nOperateFn == 20) || //weapon rack
		(obj->nOperateFn == 33) || //writ
		(obj->nOperateFn == 48) || //trapped soul
		(obj->nOperateFn == 51) || //stash
		(obj->nOperateFn == 68)    //evil urn
		));
}

const static std::array<BYTE, 11> filteredMagicItems = {
	ITEM_TEXT_TYPE_SHIELD,
	ITEM_TEXT_TYPE_ARMOR,
	ITEM_TEXT_TYPE_RING,
	ITEM_TEXT_TYPE_AMULET,
	ITEM_TEXT_TYPE_JEWEL,
	ITEM_TEXT_TYPE_CIRCLET,
	ITEM_TEXT_TYPE_SMALL_CHARM,
	ITEM_TEXT_TYPE_LARGE_CHARM,
	ITEM_TEXT_TYPE_GRAND_CHARM,
	ITEM_TEXT_TYPE_AMAZON_BOW,
	ITEM_TEXT_TYPE_AMAZON_JAV,
};

static std::string regularFmtStr = "%s%s L%d";
static std::string ethFmtStr = regularFmtStr +" (Eth)";
static std::string socketFmtStr = regularFmtStr + " (%d)";
static std::string ethSocketFmtStr = ethFmtStr + " (%d)";

void DrawAutomapPrimitives() {
	UnitAny* player = D2CLIENT_GetPlayerUnit();
	if (!player || !player->pAct || !player->pPath ||
      !player->pPath->pRoom1 || !player->pPath->pRoom1->pRoom2 ||
      !player->pPath->pRoom1->pRoom2->pLevel || player->pPath->pRoom1->pRoom2->pLevel->dwLevelNo == 0)
		return;

	// Monster Tracking
  for (int j = 0; j < 128; ++j) {
    for (UnitAny* unit = p_D2CLIENT_ServerSideUnitHashTables[UNIT_MONSTER].table[j]; unit; unit = unit->pListNext) {
      DWORD xPos, yPos;
      if (unit->dwType == UNIT_MONSTER && IsValidMonster(unit)) {
        int color = 0x5B; // Red
				if (unit->pMonsterData->fBoss & 1) color = 0x84; // Green
				if (unit->pMonsterData->fChamp & 1) color = 0x91; // Blue
				if (unit->pMonsterData->fMinion & 1) color = 0x60; // Orange

        xPos = unit->pPath->xPos;
        yPos = unit->pPath->yPos;

        POINT automapLoc = ScreenToAutomap(xPos, yPos);
        DrawCrosshook(automapLoc.x, automapLoc.y, color);
      } 
    }
	}

	// Object tracking
  for (int j = 0; j < 128; ++j) {
    for (UnitAny* unit = p_D2CLIENT_ServerSideUnitHashTables[UNIT_OBJECT].table[j]; unit; unit = unit->pListNext) {
      DWORD xPos, yPos;
			if (unit->dwType == UNIT_OBJECT && !unit->dwMode && IsObjectChest(unit->pObjectData->pTxt)) {
        xPos = unit->pObjectPath->dwPosX;
        yPos = unit->pObjectPath->dwPosY;
        auto automapLoc = ScreenToAutomap(xPos, yPos);
        DrawBox(automapLoc.x - 1, automapLoc.y - 1, 4, 4, 255, BoxTrans::BTHighlight);
      }
    }
	}

	// Debug Item Tracking
	//for (int j = 0; j < 128; j++) {
	//	for (UnitAny* unit = p_D2CLIENT_ServerSideUnitHashTables[UNIT_ITEM].table[j]; unit; unit = unit->pListNext) {
	//		if (unit->dwType == UNIT_ITEM &&
  //        (unit->dwMode == ITEM_MODE_BEING_DROPPED)) {
  //        ItemTxt* txt = D2COMMON_GetItemText(unit->dwTxtFileNo);
	//				if (txt) {
  //          PrintText(TextColor::Red, "Type: %s%d", GREEN_COLOR_REPLACEMENT, txt->nType);
	//				}
	//		}
	//	}
	//}

	// Item tracking
  for (int j = 0; j < 128; j++) {
    for (UnitAny* unit = p_D2CLIENT_ServerSideUnitHashTables[UNIT_ITEM].table[j]; unit; unit = unit->pListNext) {
			if (unit->dwType == UNIT_ITEM &&
          (unit->dwFlags & UNITFLAG_NO_EXPERIENCE) == 0x0 &&
          (unit->dwMode == ITEM_MODE_ON_GROUND || unit->dwMode == ITEM_MODE_BEING_DROPPED)) {
        unit->dwFlags |= UNITFLAG_REVEALED;

        bool overRideQualityFilter = false;
				bool isEth = false;
				int numSockets = 0;
        const char* colorReplacement = WHITE_COLOR_REPLACMENT;

        if (unit->pItemData->dwFlags & ITEM_FLAG_HASSOCKETS) {
          overRideQualityFilter = true;
          colorReplacement = GRAY_COLOR_REPLACEMENT;
          numSockets = D2COMMON_GetUnitStat(unit, STAT_SOCKETS, 0);
        }
        if (unit->pItemData->dwFlags & ITEM_FLAG_ETHEREAL) {
          overRideQualityFilter = true;
					isEth = true;
          colorReplacement = GRAY_COLOR_REPLACEMENT;
        }

        ItemTxt* txt = D2COMMON_GetItemText(unit->dwTxtFileNo);
        if (txt) {
          switch (txt->nType) {
            case ITEM_TEXT_TYPE_RUNE:
            case ITEM_TEXT_TYPE_QUEST:
              colorReplacement = GOLD_COLOR_REPLACEMENT;
              overRideQualityFilter = true;
              break;
            case ITEM_TEXT_TYPE_SMALL_CHARM:
            case ITEM_TEXT_TYPE_LARGE_CHARM:
            case ITEM_TEXT_TYPE_GRAND_CHARM: 
              colorReplacement = BLUE_COLOR_REPLACEMENT;
              overRideQualityFilter = true;
              break;
            case ITEM_TEXT_TYPE_AMETHYST_GEM:
            case ITEM_TEXT_TYPE_DIAMOND_GEM:
            case ITEM_TEXT_TYPE_EMERALD_GEM:
            case ITEM_TEXT_TYPE_RUBY_GEM:
            case ITEM_TEXT_TYPE_SAPPHIRE_GEM:
            case ITEM_TEXT_TYPE_TOPAZ_GEM:
            case ITEM_TEXT_TYPE_SKULL:
              colorReplacement = PURPLE_COLOR_REPLACEMENT;
              overRideQualityFilter = true;
              break;
          }

					switch (unit->pItemData->dwQuality) {
            case ITEM_QUALITY_MAGIC: 
							if (!std::binary_search(filteredMagicItems.begin(), filteredMagicItems.end(), txt->nType)) {
                unit->dwFlags |= UNITFLAG_NO_EXPERIENCE;
								continue;
							}
							colorReplacement = BLUE_COLOR_REPLACEMENT;
              break;
            case ITEM_QUALITY_SET:
							colorReplacement = GREEN_COLOR_REPLACEMENT;
							break;
            case ITEM_QUALITY_RARE:
							colorReplacement = YELLOW_COLOR_REPLACEMENT;
              break;
            case ITEM_QUALITY_UNIQUE:
							colorReplacement = GOLD_COLOR_REPLACEMENT;
							break;
            default:
              if (!overRideQualityFilter && (unit->pItemData->dwQuality <= ITEM_QUALITY_SUPERIOR)) {
                unit->dwFlags |= UNITFLAG_NO_EXPERIENCE;
                continue;
              }
					}
        }

        std::string itemName = GetItemName(unit);
				if (isEth && (numSockets > 0)) {
          PrintText(TextColor::Red, ethSocketFmtStr.c_str(), colorReplacement, itemName.c_str(), unit->pItemData->dwItemLevel, numSockets);
				}
				else if (isEth) {
          PrintText(TextColor::Red, ethFmtStr.c_str(), colorReplacement, itemName.c_str(), unit->pItemData->dwItemLevel);
				}
				else if (numSockets > 0) {
          PrintText(TextColor::Red, socketFmtStr.c_str(), colorReplacement, itemName.c_str(), unit->pItemData->dwItemLevel, numSockets);
				}
				else {
          PrintText(TextColor::Red, regularFmtStr.c_str(), colorReplacement, itemName.c_str(), unit->pItemData->dwItemLevel);
				}
        unit->dwFlags |= UNITFLAG_NO_EXPERIENCE;
      }
    }
	}
}

void GameAutomapDraw(void) {
	DrawAutomapPrimitives();
}

void __declspec(naked) GameAutomapDraw_Interception()
{
	__asm
	{
		push eax;
		call GameAutomapDraw;
		pop eax;
		pop edi;
		pop esi;
		ret;
	}
}
