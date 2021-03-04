#include "GameFrame.h"

GameFrame::GameFrame()
{
}

void GameFrame::SetGame(HDC hdc)
{
	//리소스등록
	Res_MG::GetInstance()->Init(hdc);
	//게임프레임창설정
	RECT rt = { 0, 0, WIN_X, WIN_Y };
	m_hFrameDC = CreateCompatibleDC(hdc);
	m_hFrame = CreateCompatibleBitmap(hdc, WIN_X, WIN_Y);
	m_hOldFrame = (HBITMAP)SelectObject(m_hFrameDC, m_hFrame);
	FillRect(m_hFrameDC, &rt, (HBRUSH)GetStockObject(GRAY_BRUSH));
	//기본맵설정
	char buf[256];
	for (int i = 0; i < MAP_MAX; i++)
	{
		wsprintf(buf, "Map%d.txt", i + 1);
		m_arrMap[i].SetMap(buf);
	}
	//게임초기설정
	m_bGameState = false;
	m_iLevel = NULL;
}

void GameFrame::UpdateGame(HDC hdc)
{
	if (clock() - m_iClock >= 50)
	{
		Draw(hdc);
		m_iClock = clock();
	}
}

void GameFrame::Draw(HDC hdc)
{
	if (m_bGameState == false)//게임시작전
	{
		POINT tmpPoint = { 0, 0 };
		SIZE tmpSize = { WIN_X, WIN_Y };
		Res_MG::GetInstance()->Draw(IMAGE_BACK, m_hFrameDC, tmpPoint, tmpSize);
		RECT rt = { 0, 0, WIN_X, WIN_Y };
		if (GetKeyState(VK_RETURN) & 0x8000)
		{
			m_bGameState = true;
			FillRect(m_hFrameDC, &rt, (HBRUSH)GetStockObject(GRAY_BRUSH));
		}
	}
	else//게임시작후
	{
		m_arrMap[m_iLevel].DrawMap(m_hFrameDC);
	}
	BitBlt(hdc, 0, 0, WIN_X, WIN_Y, m_hFrameDC, 0, 0, SRCCOPY);
}

void GameFrame::EndGame()
{

}

GameFrame::~GameFrame()
{
	DeleteObject(m_hFrame);
	SelectObject(m_hFrameDC, m_hOldFrame);
	DeleteDC(m_hFrameDC);
}