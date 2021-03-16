#pragma once
#include"Map.h"

class MapTool
{
private:
	static MapTool* m_hThis;
	Map m_Map;
	Tile m_arrTile[MAP][MAP];
	Tile m_arrTileSet[TILE_MAX];
	Tile m_Select;
public:
	static MapTool* GetInstance()
	{
		if (m_hThis == NULL)
			m_hThis = new MapTool;
		return m_hThis;
	}
	void Reset();
	void DrawMapTool(HDC hdc);
	bool Click(POINT pt);
	void Save(HWND hDlg);
	void Load(HWND hDlg);
	MapTool();
	~MapTool();
};

