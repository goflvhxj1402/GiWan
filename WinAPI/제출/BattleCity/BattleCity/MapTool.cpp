#include "MapTool.h"

MapTool* MapTool::m_hThis = NULL;
MapTool::MapTool()
{
	Reset();
}

void MapTool::Reset()
{
	//타일셋설정
	POINT tmpPoint = { 750, 50 };
	for (int i = IMAGE_EMPTY; i < IMAGE_EMPTY + TILE_MAX; i++)
	{
		m_arrTileSet[i - 1].m_iIndex = i;
		m_arrTileSet[i - 1].m_Point = tmpPoint;
		m_arrTileSet[i - 1].m_Rect = { m_arrTileSet[i - 1].m_Point.x, m_arrTileSet[i - 1].m_Point.y,
			m_arrTileSet[i - 1].m_Point.x + IMAGE_SIZE, m_arrTileSet[i - 1].m_Point.y + IMAGE_SIZE };
		if (i >= IMAGE_BLOCK1 && i <= IMAGE_BLOCK1_RIGHT)
			m_arrTileSet[i - 1].m_iTileState = TILE_STATE_BREAK;
		else if(i >= IMAGE_BLOCK2 && i <= IMAGE_BLOCK2_RIGHT)
			m_arrTileSet[i - 1].m_iTileState = TILE_STATE_FIX;
		else if(i == IMAGE_ITEM)
			m_arrTileSet[i - 1].m_iTileState = TILE_STATE_ITEM;
		else if(i == IMAGE_BASE)
			m_arrTileSet[i - 1].m_iTileState = TILE_STATE_BASE;
		else
			m_arrTileSet[i - 1].m_iTileState = TILE_STATE_NONE;
		if ((i - 1) % 2 == 1)
		{
			tmpPoint.x = 750;
			tmpPoint.y += IMAGE_SIZE;
		}
		else
			tmpPoint.x += IMAGE_SIZE;
	}
	//맵판설정
	m_Map.SetMap();
}

void MapTool::DrawMapTool(HDC hdc)
{
	//타일셋그리기
	for (int i = 0; i < TILE_MAX; i++)
		Res_MG::GetInstance()->Draw(m_arrTileSet[i].m_iIndex, hdc, m_arrTileSet[i].m_Point, PUBLIC_SIZE);
	//맵그리기
	m_Map.DrawMap(hdc, MAPTOOL);
}

bool MapTool::Click(POINT pt)
{
	//타일인덱스설정
	for (int i = 0; i < TILE_MAX; i++)
	{
		if (PtInRect(&m_arrTileSet[i].m_Rect, pt))
		{
			m_Select = m_arrTileSet[i];
			return true;
		}
	}
	//맵에타일적용시키기
	for (int y = 0; y < MAP; y++)
	{
		for (int x = 0; x < MAP; x++)
		{
			m_Map.m_arrTile[y][x].m_ImageRect = m_arrTile[y][x].m_Rect;
			if (PtInRect(&m_Map.m_arrTile[y][x].m_Rect, pt))
			{
				switch (m_Select.m_iIndex)
				{
				case IMAGE_BLOCK1_UP:
				case IMAGE_BLOCK2_UP:
					m_Map.m_arrTile[y][x].m_ImageRect.bottom -= IMAGE_SIZE / 2;
					break;
				case IMAGE_BLOCK1_LEFT:
				case IMAGE_BLOCK2_LEFT:
					m_Map.m_arrTile[y][x].m_ImageRect.right -= IMAGE_SIZE / 2;
					break;
				case IMAGE_BLOCK1_DOWN:
				case IMAGE_BLOCK2_DOWN:
					m_Map.m_arrTile[y][x].m_ImageRect.top += IMAGE_SIZE / 2;
					break;
				case IMAGE_BLOCK1_RIGHT:
				case IMAGE_BLOCK2_RIGHT:
					m_Map.m_arrTile[y][x].m_ImageRect.left += IMAGE_SIZE / 2;
					break;
				}
				m_Map.m_arrTile[y][x].m_iIndex = m_Select.m_iIndex;
				m_Map.m_arrTile[y][x].m_iTileState = m_Select.m_iTileState;
				return true;
			}
		}
	}
	return false;
}

void MapTool::Save(HWND hDlg)
{
	OPENFILENAME OFN;
	char lpstrFile[MAX_PATH] = "";
	char lpstrPath[MAX_PATH] = "";
	memset(&OFN, NULL, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hDlg;
	OFN.lpstrFilter = "Text File\0*.txt;*.doc\0";
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	GetCurrentDirectory(MAX_PATH, lpstrPath);
	OFN.lpstrInitialDir = lpstrPath;

	if (GetSaveFileName(&OFN) == 0)
	{
		DWORD err = CommDlgExtendedError();
		return;
	}

	HANDLE hFile;
	DWORD Write;
	hFile = CreateFile(OFN.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	for (int y = 0; y < MAP; y++)
		for (int x = 0; x < MAP; x++)
			WriteFile(hFile, &m_Map.m_arrTile[y][x], sizeof(Tile), &Write, NULL);
	CloseHandle(hFile);
}

void MapTool::Load(HWND hDlg)
{
	OPENFILENAME OFN;
	char lpstrFile[MAX_PATH] = "";
	char lpstrPath[MAX_PATH] = "";
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hDlg;
	OFN.lpstrFilter = "Text File\0*.txt;*.doc\0";
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	GetCurrentDirectory(MAX_PATH, lpstrPath);
	OFN.lpstrInitialDir = lpstrPath;

	if (GetSaveFileName(&OFN) == 0)
	{
		DWORD err = CommDlgExtendedError();
		return;
	}

	HANDLE hFile;
	DWORD Read;
	hFile = CreateFile(OFN.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	memset(m_Map.m_arrTile, NULL, sizeof(m_Map.m_arrTile));
	for (int y = 0; y < MAP; y++)
		for (int x = 0; x < MAP; x++)
			ReadFile(hFile, &m_Map.m_arrTile[y][x], sizeof(Tile), &Read, NULL);
	CloseHandle(hFile);
}

void MapTool::SetMap(HWND hDlg, Map& Map)
{
	OPENFILENAME OFN;
	char lpstrFile[MAX_PATH] = "";
	char lpstrPath[MAX_PATH] = "";
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hDlg;
	OFN.lpstrFilter = "Text File\0*.txt;*.doc\0";
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	GetCurrentDirectory(MAX_PATH, lpstrPath);
	OFN.lpstrInitialDir = lpstrPath;

	if (GetSaveFileName(&OFN) == 0)
	{
		DWORD err = CommDlgExtendedError();
		return;
	}

	HANDLE hFile;
	DWORD Read;
	hFile = CreateFile(OFN.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	memset(Map.m_arrTile, NULL, sizeof(Map.m_arrTile));
	for (int y = 0; y < MAP; y++)
		for (int x = 0; x < MAP; x++)
			ReadFile(hFile, &Map.m_arrTile[y][x], sizeof(Tile), &Read, NULL);
	CloseHandle(hFile);
	Map.SetImageRect();
}

MapTool::~MapTool()
{
}