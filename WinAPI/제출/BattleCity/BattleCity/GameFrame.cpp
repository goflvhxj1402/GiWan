#include "GameFrame.h"

GameFrame::GameFrame()
{
	
}

void GameFrame::SetGame(HDC hdc)
{
	//리소스등록
	Res_MG::GetInstance()->Init(hdc);
	//게임프레임설정
	m_hFrameDC = CreateCompatibleDC(hdc);
	m_hFrame = CreateCompatibleBitmap(hdc, WIN_X, WIN_Y);
	m_hOldFrame = (HBITMAP)SelectObject(m_hFrameDC, m_hFrame);
	//맵설정
	char buf[256];
	for (int i = 0; i < MAP_MAX; i++)
	{
		wsprintf(buf, "Map%d.txt", i + 1);
		m_arrMap[i].SetMap(buf);
	}	
	//게임초기설정
	m_iClock = clock();
	m_iGameState = GAME_STATE_FALSE;
	m_iLevel = 0;
	//플레이어 및 적 탱크설정
	m_parrUnit[ENEMY] = new Enemy;
	m_parrUnit[ENEMY]->SetUnit(m_iLevel);
	m_parrUnit[PLAYER] = new Player;
	m_parrUnit[PLAYER]->SetUnit(m_iLevel);
}

void GameFrame::SetCustomMap(HWND hWnd, int MapNum)
{
	MapTool::GetInstance()->SetMap(hWnd, m_arrMap[MapNum]);
}

void GameFrame::UpdateGame(HWND hWnd, HDC hdc)
{
	if (clock() - m_iClock >= 50)
	{
		if (m_iGameState == GAME_STATE_TRUE)
			GameCheck(hWnd, hdc);
		Draw(hdc);
		m_iClock = clock();
	}
}

void GameFrame::GameCheck(HWND hWnd, HDC hdc)
{
	m_parrUnit[ENEMY]->CreateTank();
	//게임진행상황확인
	//플레이어확인
	if (m_parrUnit[PLAYER]->GameCheck(m_iLevel) == true)
	{
		if (MessageBox(hWnd, "패배...다시하시겠습니까?", "패배..", MB_OKCANCEL) == IDOK)
			SetGame(hdc);
		else
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
		return;
	}
	//적탱크확인
	if (m_parrUnit[ENEMY]->GameCheck(m_iLevel) == true)
		m_parrUnit[PLAYER]->SetUnit(m_iLevel);
	//최종스테이지클리어
	if (m_iLevel >= 5)
	{
		if (MessageBox(hWnd, "승리!!!다시하시겠습니까?", "승리!!", MB_OKCANCEL) == IDOK)
			SetGame(hdc);
		else
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
		return;
	}
}

void GameFrame::Draw(HDC hdc)
{
	if (m_iGameState == GAME_STATE_FALSE)//게임대기상태
	{
		Res_MG::GetInstance()->Draw(IMAGE_LOBBY, m_hFrameDC, ORIGIN_PT, WIN_SIZE);
		if (GetKeyState(VK_RETURN) & 0x8000)
			m_iGameState = GAME_STATE_PAUSE;
	}
	else if (m_iGameState == GAME_STATE_PAUSE)//게임스테이지상승 또는 대기
	{
		char buf[256];
		m_hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, "tmp");
		m_hOldFont = (HFONT)SelectObject(m_hFrameDC, m_hFont);
		Res_MG::GetInstance()->Draw(IMAGE_BOARD, m_hFrameDC, ORIGIN_PT, WIN_SIZE);
		Res_MG::GetInstance()->Draw(IMAGE_STAGE, m_hFrameDC, STAGE_PT, PUBLIC_SIZE);
		wsprintf(buf, "STAGE %d", m_iLevel + 1);
		SetBkMode(m_hFrameDC, TRANSPARENT);
		TextOut(m_hFrameDC, STAGE_PT.x + PUBLIC_SIZE.cx + HALF, STAGE_PT.y + HALF, buf, strlen(buf));
		TextOut(m_hFrameDC, STAGE_PT.x / 1.5f, STAGE_PT.y * 2, "계속하시려면 Enter키를 누르세요", strlen("계속하시려면 Enter키를 누르세요"));
		if (GetKeyState(VK_RETURN) & 0x8000)
			m_iGameState = GAME_STATE_TRUE;
		SelectObject(m_hFrameDC, m_hOldFont);
		DeleteObject(m_hFont);
	}
	else//게임진행중
	{
		Res_MG::GetInstance()->Draw(IMAGE_BOARD, m_hFrameDC, ORIGIN_PT, WIN_SIZE);
		m_arrMap[m_iLevel].DrawMap(m_hFrameDC, NORMAL);
		m_parrUnit[ENEMY]->UpdateUnit(m_hFrameDC, m_arrMap[m_iLevel]);
		m_parrUnit[PLAYER]->UpdateUnit(m_hFrameDC, m_arrMap[m_iLevel]);
	}
	//고속복사
	BitBlt(hdc, 0, 0, WIN_X, WIN_Y, m_hFrameDC, 0, 0, SRCCOPY);
}

void GameFrame::EndGame()
{
	delete m_parrUnit[ENEMY];
	delete m_parrUnit[PLAYER];
	delete Res_MG::GetInstance();
}

GameFrame::~GameFrame()
{
	DeleteObject(m_hFrame);
	SelectObject(m_hFrameDC, m_hOldFrame);
	DeleteDC(m_hFrameDC);
}