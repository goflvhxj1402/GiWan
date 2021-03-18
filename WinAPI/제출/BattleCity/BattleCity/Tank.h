#pragma once
#include"Map.h"

static RECT g_PlayerRect;
static RECT g_arrEnemyRect[3];
static int Index = EMPTY;
class Tank
{
protected:
	int m_iLife;
	int m_iIcon;
	int m_iSpeed;
	POINT m_IconPt;
public:
	void BulletBoom(HDC hdc, TankInfo& Tank);
	void GameInfoDraw(HDC hdc);
	virtual void ResetTank(int Level) = 0;
	virtual void Draw(HDC hdc) = 0;
	virtual void Move(Map& Map) = 0;
	virtual void Attack(Map& Map) = 0;
	virtual void Die() = 0;
	virtual bool GameCheck(int& Level) = 0;
	virtual void CreateTank() = 0;
	virtual bool RectCheck(int x, int y, int Index = EMPTY) = 0;
	Tank();
	~Tank();
};

//플레이어
class Player : public Tank
{
private:
	TankInfo m_Tank;
	POINT m_DefaultPt;
	int m_iItem;
	int m_iItemClock;
public:
	void ResetTank(int Level);
	void Draw(HDC hdc);
	void Move(Map& Map);
	void Attack(Map& Map);
	void Die();
	bool GameCheck(int& Level);
	void CreateTank();
	bool RectCheck(int x, int y, int Index = EMPTY);
	Player();
	~Player();
};
//플레이어

//적
class Enemy : public Tank
{
private:
	vector<TankInfo> m_vEnemy;
	POINT m_arrDefaultPt[3];
	int m_iCreateMax;
	int m_iCreateClock;
	int m_iLevelTime;
	int m_iMoveTime;
	int m_iCount;
public:
	void ResetTank(int Level);
	void CreateTank();
	bool RectCheck(int x, int y, int Index = EMPTY);
	void Draw(HDC hdc);
	void Move(Map& Map);
	void Attack(Map& Map);
	void Die();
	bool GameCheck(int& Level);
	Enemy();
	~Enemy();
};
//적