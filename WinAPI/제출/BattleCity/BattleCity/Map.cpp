#include "Map.h"

Map::Map()
{	
}

void Map::SetMap(LPCSTR buf)
{
	for (int y = 0; y < MAP; y++)
	{
		for (int x = 0; x < MAP; x++)
		{
			m_arrTile[y][x].m_iIndex = EMPTY;
			m_arrTile[y][x].m_Point = { (x + 1) * IMAGE_SIZE, (y + 1) * IMAGE_SIZE };
			m_arrTile[y][x].m_Rect = { m_arrTile[y][x].m_Point.x, m_arrTile[y][x].m_Point.y,
			m_arrTile[y][x].m_Point.x + IMAGE_SIZE, m_arrTile[y][x].m_Point.y + IMAGE_SIZE };
			m_arrTile[y][x].m_ImageRect = m_arrTile[y][x].m_Rect;
		}
	}
	if (buf != "Empty")
	{
		HANDLE hFile = CreateFile(buf, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		DWORD Read;
		for (int y = 0; y < MAP; y++)
			for (int x = 0; x < MAP; x++)
				ReadFile(hFile, &m_arrTile[y][x], sizeof(Tile), &Read, NULL);
		CloseHandle(hFile);
	}
	SetImageRect();
}

void Map::DrawMap(HDC hdc, int Option)
{
	if (Option == NORMAL)//뒷판그리기
	{
		for (int y = 0; y < MAP; y++)
			for (int x = 0; x < MAP; x++)
				Res_MG::GetInstance()->Draw(IMAGE_EMPTY, hdc, m_arrTile[y][x].m_Point, PUBLIC_SIZE);
	}
	for (int y = 0; y < MAP; y++)//맵그리기
	{
		for (int x = 0; x < MAP; x++)
		{
			if (m_arrTile[y][x].m_iIndex == EMPTY)
				Rectangle(hdc, m_arrTile[y][x].m_Rect.left, m_arrTile[y][x].m_Rect.top, m_arrTile[y][x].m_Rect.right, m_arrTile[y][x].m_Rect.bottom);
			else
				Res_MG::GetInstance()->Draw(m_arrTile[y][x].m_iIndex, hdc, m_arrTile[y][x].m_Point, PUBLIC_SIZE);
		}
	}
}

void Map::BushDraw(HDC hdc)
{
	for (int y = 0; y < MAP; y++)
	{
		for (int x = 0; x < MAP; x++)
		{
			if(m_arrTile[y][x].m_iIndex == IMAGE_BLOCK_GRASS)
				Res_MG::GetInstance()->Draw(IMAGE_BLOCK_GRASS, hdc, m_arrTile[y][x].m_Point, PUBLIC_SIZE);
		}
	}
}

void Map::SetImageRect()
{
	for (int y = 0; y < MAP; y++)
	{
		for (int x = 0; x < MAP; x++)
		{
			m_arrTile[y][x].m_ImageRect = m_arrTile[y][x].m_Rect;
			switch (m_arrTile[y][x].m_iIndex)
			{
			case IMAGE_BLOCK1_UP:
			case IMAGE_BLOCK2_UP:
				m_arrTile[y][x].m_ImageRect.bottom -= HALF;
				break;
			case IMAGE_BLOCK1_LEFT:
			case IMAGE_BLOCK2_LEFT:
				m_arrTile[y][x].m_ImageRect.right -= HALF;
				break;
			case IMAGE_BLOCK1_DOWN:
			case IMAGE_BLOCK2_DOWN:
				m_arrTile[y][x].m_ImageRect.top += HALF;
				break;
			case IMAGE_BLOCK1_RIGHT:
			case IMAGE_BLOCK2_RIGHT:
				m_arrTile[y][x].m_ImageRect.left += HALF;
				break;
			}
		}
	}
}

int Map::MoveCheck(int x, int y, int Option)
{
	RECT tmp1, tmp2;
	tmp2 = { x, y, x + IMAGE_SIZE, y + IMAGE_SIZE };
	if (x >= 50 && x <= 650 && y >= 50 && y <= 650)
	{
		for (int y = 0; y < MAP; y++)
		{
			for (int x = 0; x < MAP; x++)
			{
				if (IntersectRect(&tmp1, &tmp2, &m_arrTile[y][x].m_ImageRect))
				{
					if (m_arrTile[y][x].m_iTileState == TILE_STATE_BREAK || m_arrTile[y][x].m_iTileState == TILE_STATE_FIX)
						return MOVE_FALSE;
 					if (m_arrTile[y][x].m_iTileState == TILE_STATE_ITEM && Option == PLAYER)
					{
						m_arrTile[y][x].m_iTileState = TILE_STATE_NONE;
						m_arrTile[y][x].m_iIndex = IMAGE_EMPTY;
						return MOVE_ITEM;
					}
					if (m_arrTile[y][x].m_iTileState == TILE_STATE_BASE && Option == ENEMY)
					{
						m_arrTile[y][x].m_iTileState = TILE_STATE_NONE;
						m_arrTile[y][x].m_iIndex = IMAGE_EMPTY;
						return MOVE_BASE;
					}
				}
			}
		}
		return MOVE_TRUE;
	}
	return MOVE_FALSE;
}

bool Map::BreakCheck(int x, int y, int Direction)//true : 총알이 사라지게함, false : 총알이 계속나가게함
{
	RECT tmp1, tmp2;
	tmp2 = { x, y, x + 10, y + 10 };
	if (x >= 50 && x <= 675 && y >= 50 && y <= 675)
	{
		for (int y = 0; y < MAP; y++)
		{
			for (int x = 0; x < MAP; x++)
			{
				if (IntersectRect(&tmp1, &tmp2, &m_arrTile[y][x].m_ImageRect))
				{
					if (m_arrTile[y][x].m_iTileState == TILE_STATE_FIX)
						return true;
					if (m_arrTile[y][x].m_iTileState == TILE_STATE_BREAK)
					{
						if (m_arrTile[y][x].m_iIndex == IMAGE_BLOCK1)
						{
							switch (Direction)
							{
							case DIRECTION_UP:
								m_arrTile[y][x].m_iIndex = IMAGE_BLOCK1_UP;
								break;
							case DIRECTION_DOWN:
								m_arrTile[y][x].m_iIndex = IMAGE_BLOCK1_DOWN;
								break;
							case DIRECTION_LEFT:
								m_arrTile[y][x].m_iIndex = IMAGE_BLOCK1_LEFT;
								break;
							case DIRECTION_RIGHT:
								m_arrTile[y][x].m_iIndex = IMAGE_BLOCK1_RIGHT;
								break;
							}
						}
						else
						{
							m_arrTile[y][x].m_iIndex = IMAGE_EMPTY;
							m_arrTile[y][x].m_iTileState = TILE_STATE_NONE;
						}
						SetImageRect();
						return true;
					}
				}
			}
		}
		return false;
	}
	return true;
}

Map::~Map()
{
}