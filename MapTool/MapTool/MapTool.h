#pragma once
#include"Map.h"

class MapTool
{
private:
	static MapTool* m_hThis;
	Map m_Map;
	Room m_arrButton[BUTTON_MAX];
	int m_iTileSelect;
public:
	static MapTool* GetInstance()
	{
		if (m_hThis == NULL)
			m_hThis = new MapTool;
		return m_hThis;
	}
	void RoomDraw(HDC hdc);
	bool Click(POINT pt);	
	void Reset();
	void Save(HWND hWnd);
	void Load(HWND hWnd);
	MapTool();
	~MapTool();
};

