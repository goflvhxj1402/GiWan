#pragma once
#include"Map.h"

class Tank
{
private:
	int m_iUnitType;
	int m_arriIndex[DIRECTION_MAX];
	int m_arriItemIndex[DIRECTION_MAX];
	int m_iMoveDir;
	int m_iAttackDir;
	int m_iSpeed;
	int m_iTankState;
	int m_iFindClock;
	int m_iMoveClock;
	int m_iDelay;
	int m_iItem;
	POINT m_TankPt;
	RECT m_TankRect;
	RECT m_PublicRect;
	RECT m_AttackRange;
	Bullet m_Bullet;
public:
	RECT ResetTank(int UnitType, POINT pt);
	void DrawTank(HDC hdc);
	RECT Move(Map Map, RECT* Rect);
	RECT Move(int Direction, Map& Map, RECT* Rect);
	void SetBullet();
	RECT Attack(Map& Map);
	void BulletBoom(HDC hdc);
	bool CollideCheck(RECT PlayerRect, RECT Rect);
	inline void SetBulletState(int State) { m_Bullet.m_iBulletState = BULLET_BREAK; }
	inline int GetItemState() { return m_iItem; }
	inline void SetItemState(int State) { m_iItem = State; }
	inline RECT GetTankRect() { return m_TankRect; }
	Tank();
	~Tank();
};