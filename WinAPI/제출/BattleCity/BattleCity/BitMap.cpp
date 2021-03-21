#include "BitMap.h"

BitMap::BitMap()
{
}

void BitMap::Init(HDC hdc, LPCSTR FileName)
{
	m_hMemDC = CreateCompatibleDC(hdc);
	m_hBit = (HBITMAP)LoadImage(NULL, FileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	m_hOld = (HBITMAP)SelectObject(m_hMemDC, m_hBit);

	BITMAP BitMap;
	GetObject(m_hBit, sizeof(BITMAP), &BitMap);
	m_Size.cx = BitMap.bmWidth;
	m_Size.cy = BitMap.bmHeight;
}

void BitMap::Draw(HDC hdc, POINT pt, SIZE Size)
{
	TransparentBlt(hdc, pt.x, pt.y, Size.cx, Size.cy, m_hMemDC, 0, 0, m_Size.cx, m_Size.cy, RGB(255, 0, 255));
}
BitMap::~BitMap()
{
	DeleteObject(m_hBit);
	SelectObject(m_hMemDC, m_hOld);
	DeleteDC(m_hMemDC);
}