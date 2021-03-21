#include "Res_MG.h"

Res_MG* Res_MG::m_hThis = NULL;
Res_MG::Res_MG()
{
}

void Res_MG::Init(HDC hdc)
{
	char buf[256];
	for (int i = IMAGE_START; i < IMAGE_MAX; i++)
	{
		wsprintf(buf, "BattleCity/%d.bmp", i);
		m_arrImage[i].Init(hdc, buf);
	}
}

void Res_MG::Draw(int Index, HDC hdc, POINT pt, SIZE Size)
{
	m_arrImage[Index].Draw(hdc, pt, Size);
}

Res_MG::~Res_MG()
{
}