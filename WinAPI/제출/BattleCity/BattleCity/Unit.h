#pragma once
#include"Tank.h"
#define MAX 3

static RECT g_arrTankRect[4];//충돌확인용[0 : PLAYER] [1 ~ 3 : ENEMY]
static int g_Index = EMPTY;

class Unit
{
protected:
	int m_iIconImage;
	int m_iLife;
	POINT m_IconPoint;
	RECT m_BulletRect;
public:
	void GameInfoDraw(HDC hdc);
	bool CollideCheck(RECT Rect1, RECT Rect2);
	virtual void SetUnit(int Level) = 0;
	virtual void CreateTank() = 0;
	virtual void UpdateUnit(HDC hdc, Map& Map) = 0;
	virtual void Die() = 0;
	virtual bool GameCheck(int& Level) = 0;
	Unit();
	~Unit();
};

//플레이어
class Player : public Unit
{
private:
	Tank m_Tank;
	POINT m_RespawnPt;
	
public:
	void SetUnit(int Level);
	void CreateTank();
	void UpdateUnit(HDC hdc, Map& Map);
	void Die();
	bool GameCheck(int& Level);
	Player();
	~Player();
};
//플레이어

//적
class Enemy : public Unit
{
private:
	vector<Tank> m_vTank;
	POINT m_arrRespawnPt[MAX];
	int m_iCreateIndex;
	int m_iCreateClcok;
public:
	void SetUnit(int Level);
	void CreateTank();
	void UpdateUnit(HDC hdc, Map& Map);
	void Die();
	bool GameCheck(int& Level);
	Enemy();
	~Enemy();
};
//적