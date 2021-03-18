#pragma once
#include"MapTool.h"
#include"Unit.h"

class GameFrame
{
private:
	HDC m_hFrameDC;
	HBITMAP m_hFrame, m_hOldFrame;
	HFONT m_hFont, m_hOldFont;
	int m_iGameState;
	int m_iClock;
	int m_iLevel;
	Map m_arrMap[MAP_MAX];
	Unit* m_parrUnit[UNIT_MAX];
public:
	void SetGame(HDC hdc);
	void SetCustomMap(HWND hWnd, int MapNum);
	void UpdateGame(HWND hWnd, HDC hdc);
	void GameCheck(HWND hWnd, HDC hdc);
	void Draw(HDC hdc);
	void EndGame();
	GameFrame();
	~GameFrame();
};

