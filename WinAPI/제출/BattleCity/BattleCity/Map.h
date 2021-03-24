#pragma once
#include"Res_MG.h"

class Map
{
public:
	Tile m_arrTile[MAP][MAP];
	void SetMap(LPCSTR buf = "Empty");
	void DrawMap(HDC hdc, int Option);
	void SetImageRect();
	int MoveCheck(int x, int y, int Option);
	bool BreakCheck(int x, int y, int Direction);
	Map();
	~Map();
};

