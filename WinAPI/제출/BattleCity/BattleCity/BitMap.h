#pragma once
#include"Mecro.h"

class BitMap
{
private:
	HDC m_hMemDC;
	HBITMAP m_hBit, m_hOld;
	SIZE m_Size;
public:
	void Init(HDC hdc, LPCSTR FileName);
	void Draw(HDC hdc, POINT pt, SIZE Size);
	BitMap();
	~BitMap();
};

