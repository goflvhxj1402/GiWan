#include "Map.h"

Map::Map()
{
	for (int y = 0; y < ROOM; y++)
	{
		for (int x = 0; x < ROOM; x++)
		{
			m_arrMap[y][x].m_iIndex = IMAGE_EMPTY;
			m_arrMap[y][x].m_iState = STATE_EMPTY;
			m_arrMap[y][x].m_Point = { (x + 1) * IMG_SIZE, (y + 1) * IMG_SIZE };
			m_arrMap[y][x].m_Size = { IMG_SIZE, IMG_SIZE };
			m_arrMap[y][x].m_Rect = { m_arrMap[y][x].m_Point.x, m_arrMap[y][x].m_Point.y,
				m_arrMap[y][x].m_Point.x + m_arrMap[y][x].m_Size.cx, m_arrMap[y][x].m_Point.y + m_arrMap[y][x].m_Size.cy };
		}
	}
}

void Map::SetMap(LPCSTR FileName)
{
	HANDLE hFile;
	DWORD dWord;
	hFile = CreateFile(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	memset(m_arrMap, NULL, sizeof(m_arrMap));
	for (int y = 0; y < ROOM; y++)
		for (int x = 0; x < ROOM; x++)
			ReadFile(hFile, &m_arrMap[y][x], sizeof(Room), &dWord, NULL);
	CloseHandle(hFile);
}

void Map::DrawMap(HDC hdc)
{
	for (int y = 0; y < ROOM; y++)
	{
		for (int x = 0; x < ROOM; x++)
		{
			if (m_arrMap[y][x].m_iState == STATE_EMPTY && m_arrMap[y][x].m_iIndex == IMAGE_EMPTY)
				Rectangle(hdc, m_arrMap[y][x].m_Rect.left, m_arrMap[y][x].m_Rect.top, m_arrMap[y][x].m_Rect.right, m_arrMap[y][x].m_Rect.bottom);
			else
				Res_MG::GetInstance()->Draw(m_arrMap[y][x].m_iIndex, hdc, m_arrMap[y][x].m_Point);
		}
	}
}

Map::~Map()
{
}