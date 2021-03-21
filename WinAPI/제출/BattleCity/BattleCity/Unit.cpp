#include "Unit.h"

Unit::Unit()
{
	memset(g_arrTankRect, EMPTY, sizeof(RECT) * 4);
}

void Unit::GameInfoDraw(HDC hdc)
{
	POINT tmpPoint = m_IconPoint;
	SetTextColor(hdc, RGB(255, 187, 0));
	if (m_iIconImage == IMAGE_PLAYER_ICON)
		TextOut(hdc, tmpPoint.x - 10, tmpPoint.y - HALF, "Player Life", strlen("Player Life"));
	else
		TextOut(hdc, tmpPoint.x - 10, tmpPoint.y - HALF, "Enemy Life", strlen("Enemy Life"));
	for (int i = 0; i < m_iLife; i++)
	{
		Res_MG::GetInstance()->Draw(m_iIconImage, hdc, tmpPoint, ICON_SIZE);
		if (i % 2 == 1)
		{
			tmpPoint.x = 725;
			tmpPoint.y += HALF;
		}
		else
			tmpPoint.x += HALF;
	}
	SetTextColor(hdc, RGB(0, 0, 0));
}

bool Unit::CollideCheck(RECT Rect1, RECT Rect2)
{
	RECT tmpRect;
	if (IntersectRect(&tmpRect, &Rect1, &Rect2))
		return true;
	return false;
}

Unit::~Unit()
{
}

//플레이어
Player::Player()
{
}

void Player::SetUnit(int Level)
{
	//아이콘설정
	m_iIconImage = IMAGE_PLAYER_ICON;
	m_IconPoint = { 725, 500 };
	//리스폰 포인트설정
	m_RespawnPt = { 250, 650 };
	if (Level == 0)//초기상태일때
		m_iLife = 3;
	CreateTank();
}

void Player::CreateTank()
{
	g_arrTankRect[0] = m_Tank.ResetTank(PLAYER, m_RespawnPt);
}

void Player::UpdateUnit(HDC hdc, Map& Map)
{
	//그리기
	m_Tank.DrawTank(hdc);
	GameInfoDraw(hdc);
	//움직이기
	int Direction = EMPTY;
	if (GetKeyState(VK_LEFT) & 0x8000)
		Direction = DIRECTION_LEFT;
	else if(GetKeyState(VK_RIGHT) & 0x8000)
		Direction = DIRECTION_RIGHT;
	else if(GetKeyState(VK_UP) & 0x8000)
		Direction = DIRECTION_UP;
	else if(GetKeyState(VK_DOWN) & 0x8000)
		Direction = DIRECTION_DOWN;
	if (g_arrTankRect[0].left != DIE && g_arrTankRect[0].left != END)
		g_arrTankRect[0] = m_Tank.Move(Direction, Map, g_arrTankRect);//전역변수 플레이어Rect업데이트
	//공격
	if (GetKeyState(VK_SPACE) & 0x8000)
		m_Tank.SetBullet();
	m_BulletRect = m_Tank.Attack(Map);
	for (int i = 1; i < 4; i++)
	{
		if (CollideCheck(m_BulletRect, g_arrTankRect[i]) == true)
		{
			g_Index = i;
			m_Tank.SetBulletState(BULLET_BREAK);
			break;
		}
	}
	//죽음확인
	Die();
	//아이템확인
	m_Tank.ItemCheck(Map);
}

void Player::Die()
{
	if (g_arrTankRect[0].left == DIE)
	{
		g_arrTankRect[0] = { EMPTY, EMPTY, EMPTY, EMPTY };
		if (m_Tank.GetItemState() == IMAGE_SHIELD1 || m_Tank.GetItemState() == IMAGE_SHIELD2)
		{
			m_Tank.SetItemState(EMPTY);
			return;
		}
		SetUnit(DIE);
		m_iLife--;
	}
	if (g_arrTankRect[0].left == END)
		m_iLife = 0;
}

bool Player::GameCheck(int& Level)
{
	if (m_iLife == 0)
		return true;
	return false;
}

Player::~Player()
{
}
//플레이어

//적
Enemy::Enemy()
{
}

void Enemy::SetUnit(int Level)
{
	//아이콘설정
	m_iIconImage = IMAGE_ENEMY_ICON;
	m_IconPoint = { 725, 50 };
	//리스폰 포인트설정
	m_arrRespawnPt[0] = { 150, 50 };
	m_arrRespawnPt[1] = { 350, 50 };
	m_arrRespawnPt[2] = { 550, 50 };
	//라이프설정
	m_iLife = ENEMY_MAX / 10;
	//탱크생성시간 초기화
	m_iCreateClcok = clock();
	//탱크생성인덱스 초기화
	m_iCreateIndex = 0;
	//플레이어추적시간 주기 설정
	m_iFind = (3 - Level) * PAUSE_TIME;
	//플레이어 기지 설정
	m_PlayerBase = { 350, 650, 400, 700 };
	//벡터초기화
	while (!m_vTank.empty())
		m_vTank.pop_back();
}

void Enemy::CreateTank()
{
	if (clock() - m_iCreateClcok >= CREATE_TIME && m_iCreateIndex < 3 && m_iLife > 0)
	{
		Tank tmpTank;
		RECT tmpRect;
		int RNum;
		RNum = rand() % 3;
		tmpRect = { m_arrRespawnPt[RNum].x, m_arrRespawnPt[RNum].y, m_arrRespawnPt[RNum].x + IMAGE_SIZE, m_arrRespawnPt[RNum].y + IMAGE_SIZE };
		for (int i = 0; i < m_vTank.size(); i++)
		{
			if (m_vTank[i].CollideCheck(tmpRect, tmpRect) == true)
				return;
		}
		m_iCreateIndex++;
		m_iLife--;
		tmpTank.ResetTank(ENEMY, m_arrRespawnPt[RNum]);
		m_vTank.push_back(tmpTank);
		m_iCreateClcok = clock();
	}
}

void Enemy::UpdateUnit(HDC hdc, Map& Map)
{
	int GapX, GapY, Direction;
	for (int i = 0; i < m_vTank.size(); i++)
	{
		//그리기
		m_vTank[i].DrawTank(hdc);
		//움직이기
		g_arrTankRect[i + 1] = m_vTank[i].Move(Map, g_arrTankRect, m_iFind, m_PlayerBase);
		if (g_arrTankRect[i + 1].left == END)
		{
			g_arrTankRect[0] = { END, END, END, END };
			break;
		}
		//공격
		m_BulletRect = m_vTank[i].Attack(Map);
		if (CollideCheck(m_BulletRect, g_arrTankRect[0]) == true)//플레이어탱크파괴
		{
			g_arrTankRect[0] = { DIE, DIE, DIE, DIE };
			m_vTank[i].SetBulletState(BULLET_BREAK);
		}
		//플레이어기지파괴확인
		if (CollideCheck(m_BulletRect, m_PlayerBase) == true)//플레이어기지파괴
		{
			g_arrTankRect[0] = {END, END, END, END};
			m_vTank[i].SetBulletState(BULLET_BREAK);
		}
	}
	//죽음확인
	Die();
	//게임정보그리기
	GameInfoDraw(hdc);
}

void Enemy::Die()
{
	if (g_Index != EMPTY)
	{
		for (int i = 0; i < m_vTank.size(); i++)
		{
			if (m_vTank[i].GetTankRect().left == g_arrTankRect[g_Index].left)
			{
				m_vTank.erase(m_vTank.begin() + i);
				m_iCreateIndex--;
				memset(g_arrTankRect, EMPTY, sizeof(RECT) * 4);
				m_iCreateClcok = clock();
				break;
			}
		}
		g_Index = EMPTY;
	}
}

bool Enemy::GameCheck(int& Level)
{
	if (m_iLife == 0 && m_iCreateIndex == 0)
	{
		Level++;
		SetUnit(Level);
		return true;
	}
	return false;
}

Enemy::~Enemy()
{
}
//적