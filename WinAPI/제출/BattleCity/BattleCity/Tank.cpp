#include "Tank.h"

Tank::Tank()
{
}

void Tank::BulletBoom(HDC hdc, TankInfo& Tank)
{
	POINT tmpPoint = { Tank.m_Bullet.m_Point.x - HALF + 10, Tank.m_Bullet.m_Point.y - HALF + 10 };
	Tank.m_Bullet.m_Rect = { DIE, DIE, DIE, DIE };
	int Index = IMAGE_BULLET_BOOM1 + Tank.m_iDelay;
	Res_MG::GetInstance()->Draw(Index, hdc, tmpPoint, PUBLIC_SIZE);
	Tank.m_iDelay++;
	if (Tank.m_iDelay == 3)
	{
		Tank.m_Bullet.m_iBulletState = BULLET_NONE;
		Tank.m_Bullet.m_Point = { Tank.m_Point.x + HALF - Tank.m_Bullet.m_Size.cx, Tank.m_Point.y + HALF - Tank.m_Bullet.m_Size.cy };
		Tank.m_Bullet.m_Rect = { Tank.m_Bullet.m_Point.x, Tank.m_Bullet.m_Point.y,
			Tank.m_Bullet.m_Point.x + Tank.m_Bullet.m_Size.cx, Tank.m_Bullet.m_Point.y + Tank.m_Bullet.m_Size.cy };
		Tank.m_iDelay = 0;
	}
}

void Tank::GameInfoDraw(HDC hdc)
{
	POINT tmpPoint = m_IconPt;
	SetTextColor(hdc, RGB(255, 187, 0));
	if (m_iIcon == IMAGE_PLAYER_ICON)
		TextOut(hdc, tmpPoint.x - 10, tmpPoint.y - HALF, "Player Life", strlen("Player Life"));
	else
		TextOut(hdc, tmpPoint.x - 10, tmpPoint.y - HALF, "Enemy Life", strlen("Enemy Life"));
	for (int i = 0; i < m_iLife; i++)
	{
		Res_MG::GetInstance()->Draw(m_iIcon, hdc, tmpPoint, ICON_SIZE);
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

Tank::~Tank()
{
}

//플레이어
Player::Player()
{
	//플레이어생성초기위치설정
	m_DefaultPt = { 250, 650 };
	//아이콘설정
	m_iIcon = IMAGE_PLAYER_ICON;
	m_IconPt = { 725, 500 };
	m_iSpeed = 10;
}

void Player::ResetTank(int Level)
{
	if (Level == 0)
		m_iLife = 3;
	//탱크초기설정
	m_Tank.m_iIndex = IMAGE_PLAYER1_UP1;
	m_Tank.m_Point = m_DefaultPt;
	m_Tank.m_iTankState = TANK_STATE_STOP;
	m_Tank.m_TankRect = { m_Tank.m_Point.x, m_Tank.m_Point.y,
	m_Tank.m_Point.x + PUBLIC_SIZE.cx, m_Tank.m_Point.y + PUBLIC_SIZE.cy };
	g_PlayerRect = m_Tank.m_TankRect;
	m_iItem = EMPTY;
	m_Tank.m_iDelay = 0;
	//탱크총알초기설정
	m_Tank.m_Bullet.m_Size = { 10, 10 };
	m_Tank.m_Bullet.m_iIndex = IMAGE_BULLET;
	m_Tank.m_Bullet.m_Point = { m_Tank.m_Point.x + HALF - m_Tank.m_Bullet.m_Size.cx, m_Tank.m_Point.y + HALF - m_Tank.m_Bullet.m_Size.cy };
	m_Tank.m_Bullet.m_Rect = { m_Tank.m_Bullet.m_Point.x, m_Tank.m_Bullet.m_Point.y, m_Tank.m_Bullet.m_Point.x + 10, m_Tank.m_Bullet.m_Point.y + 10 };
	//공격상태초기설정
	m_Tank.m_Bullet.m_iBulletState = BULLET_NONE;
	m_Tank.m_iAttackDirection = EMPTY;
}

void Player::Draw(HDC hdc)
{
	//탱크이미지설정
	if (m_Tank.m_iTankState == TANK_STATE_MOVE)
	{
		if (m_Tank.m_iIndex == IMAGE_PLAYER1_DOWN1 || m_Tank.m_iIndex == IMAGE_PLAYER1_LEFT1 ||
			m_Tank.m_iIndex == IMAGE_PLAYER1_RIGHT1 || m_Tank.m_iIndex == IMAGE_PLAYER1_UP1)
			m_Tank.m_iIndex++;
		else
			m_Tank.m_iIndex--;
	}
	//아이템이미지설정
	if (clock() - m_iItemClock >= 0)
	{
		m_iItem = EMPTY;
		m_iSpeed = 10;
	}
	if (m_iItem != EMPTY)
	{
		if (m_iItem == IMAGE_SHIELD1)
		{
			m_iItem++;
			Res_MG::GetInstance()->Draw(m_iItem, hdc, m_Tank.m_Point, PUBLIC_SIZE);
		}
		else if (m_iItem == IMAGE_SHIELD2)
		{
			m_iItem--;
			Res_MG::GetInstance()->Draw(m_iItem, hdc, m_Tank.m_Point, PUBLIC_SIZE);
		}
		else
		{
			switch (m_Tank.m_iMoveDirection)
			{
			case LEFT:
				m_Tank.m_iIndex = IMAGE_PLAYER2_LEFT;
				break;
			case RIGHT:
				m_Tank.m_iIndex = IMAGE_PLAYER2_RIGHT;
				break;
			case UP:
				m_Tank.m_iIndex = IMAGE_PLAYER2_UP;
				break;
			case DOWN:
				m_Tank.m_iIndex = IMAGE_PLAYER2_DOWN;
				break;
			}
		}	
	}
	//탱크그리기
	Res_MG::GetInstance()->Draw(m_Tank.m_iIndex, hdc, m_Tank.m_Point, PUBLIC_SIZE);
	//총알그리기
	if (m_Tank.m_Bullet.m_iBulletState == BULLET_MOVE)
		Res_MG::GetInstance()->Draw(m_Tank.m_Bullet.m_iIndex, hdc, m_Tank.m_Bullet.m_Point, m_Tank.m_Bullet.m_Size);
	if (m_Tank.m_Bullet.m_iBulletState == BULLET_BREAK)
		BulletBoom(hdc, m_Tank);
	//라이프그리기
	GameInfoDraw(hdc);
}

void Player::Move(Map& Map)
{
	int AddX = 0, AddY = 0;
	m_Tank.m_iTankState = TANK_STATE_STOP;
	//움직이기부분
	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		AddX = -m_iSpeed;
		m_Tank.m_iMoveDirection = LEFT;
		if (m_Tank.m_iIndex != IMAGE_PLAYER1_LEFT1 && m_Tank.m_iIndex != IMAGE_PLAYER1_LEFT2)
			m_Tank.m_iIndex = IMAGE_PLAYER1_LEFT1;
		m_Tank.m_iTankState = TANK_STATE_MOVE;
	}
	else if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		AddX = m_iSpeed;
		m_Tank.m_iMoveDirection = RIGHT;
		if (m_Tank.m_iIndex != IMAGE_PLAYER1_RIGHT1 && m_Tank.m_iIndex != IMAGE_PLAYER1_RIGHT2)
			m_Tank.m_iIndex = IMAGE_PLAYER1_RIGHT1;
		m_Tank.m_iTankState = TANK_STATE_MOVE;
	}
	else if (GetKeyState(VK_UP) & 0x8000)
	{
		AddY = -m_iSpeed;
		m_Tank.m_iMoveDirection = UP;
		if (m_Tank.m_iIndex != IMAGE_PLAYER1_UP1 && m_Tank.m_iIndex != IMAGE_PLAYER1_UP2)
			m_Tank.m_iIndex = IMAGE_PLAYER1_UP1;
		m_Tank.m_iTankState = TANK_STATE_MOVE;
	}
	else if (GetKeyState(VK_DOWN) & 0x8000)
	{
		AddY = m_iSpeed;
		m_Tank.m_iMoveDirection = DOWN;
		if (m_Tank.m_iIndex != IMAGE_PLAYER1_DOWN1 && m_Tank.m_iIndex != IMAGE_PLAYER1_DOWN2)
			m_Tank.m_iIndex = IMAGE_PLAYER1_DOWN1;
		m_Tank.m_iTankState = TANK_STATE_MOVE;
	}	
	//움직이기충돌체크 이상없으면 움직이기 및 아이템
	int Result = Map.MoveCheck(m_Tank.m_Point.x + AddX, m_Tank.m_Point.y + AddY, PLAYER);
	if (Result != MOVE_FALSE)
	{
		if (Result == MOVE_ITEM)
		{
			if (rand() % 2 == 0)
				m_iItem = IMAGE_SHIELD1;
			else
			{
				m_iSpeed += 5;
				m_iItem = IMAGE_EMPTY;
			}
			m_iItemClock = clock() + 5000;
		}
		m_Tank.m_Point.x += AddX;
		m_Tank.m_Point.y += AddY;
		m_Tank.m_TankRect = { m_Tank.m_Point.x , m_Tank.m_Point.y, m_Tank.m_Point.x + IMAGE_SIZE , m_Tank.m_Point.y + IMAGE_SIZE };
	}
	g_PlayerRect = m_Tank.m_TankRect;
}

void Player::Attack(Map& Map)
{	
	//공격부분
	if (GetKeyState(VK_SPACE) & 0x8000 && m_Tank.m_Bullet.m_iBulletState == BULLET_NONE)
	{
		m_Tank.m_Bullet.m_iBulletState = BULLET_MOVE;
		m_Tank.m_Bullet.m_Point = { m_Tank.m_Point.x + HALF - m_Tank.m_Bullet.m_Size.cx / 2, m_Tank.m_Point.y + HALF - m_Tank.m_Bullet.m_Size.cy / 2 };
		m_Tank.m_iAttackDirection = m_Tank.m_iMoveDirection;
	}
	//탄이동
	if (m_Tank.m_Bullet.m_iBulletState == BULLET_MOVE)
	{
		switch (m_Tank.m_iAttackDirection)
		{
		case UP:
			m_Tank.m_Bullet.m_Point.y -= BULLET_SPEED;
			break;
		case DOWN:
			m_Tank.m_Bullet.m_Point.y += BULLET_SPEED;
			break;
		case LEFT:
			m_Tank.m_Bullet.m_Point.x -= BULLET_SPEED;
			break;
		case RIGHT:
			m_Tank.m_Bullet.m_Point.x += BULLET_SPEED;
			break;
		}
		m_Tank.m_Bullet.m_Rect = { m_Tank.m_Bullet.m_Point.x, m_Tank.m_Bullet.m_Point.y,
			m_Tank.m_Bullet.m_Point.x + m_Tank.m_Bullet.m_Size.cx, m_Tank.m_Bullet.m_Point.y + m_Tank.m_Bullet.m_Size.cy };
	}
	//공격충돌체크
	RECT tmpRect;
	if (m_Tank.m_Bullet.m_iBulletState != BULLET_NONE)
	{	
		//상대방충돌확인
		for (int i = 0; i < 3; i++)
		{
			if (IntersectRect(&tmpRect, &m_Tank.m_Bullet.m_Rect, &g_arrEnemyRect[i]))
			{
				Index = i;
				break;
			}
		}
		//탄 벽 부딪힘확인
		if(Map.BreakCheck(m_Tank.m_Bullet.m_Point.x, m_Tank.m_Bullet.m_Point.y, m_Tank.m_iAttackDirection) == true || Index != EMPTY)
			m_Tank.m_Bullet.m_iBulletState = BULLET_BREAK;
	}
}

void Player::Die()
{
	if (g_PlayerRect.left == DIE)
	{
		if (m_iItem == IMAGE_SHIELD1 || m_iItem == IMAGE_SHIELD2)
		{
			m_iItem = EMPTY;
			return;
		}
		ResetTank(DIE);
		m_iLife--;
	}
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
	//적생성위치 설정
	m_arrDefaultPt[0] = { 150, 50 };
	m_arrDefaultPt[1] = { 350, 50 };
	m_arrDefaultPt[2] = { 550, 50 };
	//아이콘설정
	m_iIcon = IMAGE_ENEMY_ICON;
	m_IconPt = { 725, 50 };
	m_iSpeed = 10;
}

void Enemy::ResetTank(int Level)
{
	m_iLife = ENEMY_MAX;
	while (!m_vEnemy.empty())
		m_vEnemy.pop_back();
	m_iCreateClock = clock();
	m_iCreateMax = Level + 3;
	m_iMoveTime = PAUSE_TIME + (Level + PAUSE_TIME);
	if (m_iMoveTime == (4 * PAUSE_TIME))
		m_iMoveTime = 3 * PAUSE_TIME;
	m_iLevelTime = (3 * PAUSE_TIME) - (Level * PAUSE_TIME);
	if (m_iLevelTime == 0)
		m_iLevelTime = PAUSE_TIME;
}

void Enemy::CreateTank()
{
	if (clock() - m_iCreateClock >= CREATE_TIME && m_iCreateMax > m_vEnemy.size() && m_iLife > 0)
	{
		int RNum;
		TankInfo tmpInfo;
		
		RNum = rand() % 3;
		if (RectCheck(m_arrDefaultPt[RNum].x, m_arrDefaultPt[RNum].y) == false)
			return;
		//탱크설정
		tmpInfo.m_iIndex = IMAGE_ENEMY_DOWN1;
		tmpInfo.m_Point = m_arrDefaultPt[RNum];
		tmpInfo.m_TankRect = { m_arrDefaultPt[RNum].x, m_arrDefaultPt[RNum].y, m_arrDefaultPt[RNum].x + IMAGE_SIZE, m_arrDefaultPt[RNum].y + IMAGE_SIZE };
		tmpInfo.m_iMoveDirection = DOWN;
		tmpInfo.m_AttackRange = { tmpInfo.m_TankRect.left - (IMAGE_SIZE * m_iCreateMax), tmpInfo.m_TankRect.top - (IMAGE_SIZE * m_iCreateMax),
		tmpInfo.m_TankRect.right + (IMAGE_SIZE * m_iCreateMax), tmpInfo.m_TankRect.bottom + (IMAGE_SIZE * m_iCreateMax) };
		tmpInfo.m_iDelay = 0;
		tmpInfo.m_iFindClock = clock();
		tmpInfo.m_iMoveClock = clock();
		//탄설정
		tmpInfo.m_Bullet.m_iIndex = IMAGE_BULLET;
		tmpInfo.m_Bullet.m_Size = { 10, 10 };
		tmpInfo.m_Bullet.m_Point = { tmpInfo.m_Point.x + (HALF - 10), tmpInfo.m_Point.y + (HALF - 10) };
		tmpInfo.m_Bullet.m_Rect = { tmpInfo.m_Bullet.m_Point.x, tmpInfo.m_Bullet.m_Point.y,
		tmpInfo.m_Bullet.m_Point.x + tmpInfo.m_Bullet.m_Size.cx, tmpInfo.m_Bullet.m_Point.y + tmpInfo.m_Bullet.m_Size.cy };
		tmpInfo.m_Bullet.m_iBulletState = BULLET_NONE;
		//push_back
		m_vEnemy.push_back(tmpInfo);		
		m_iCreateClock = clock();
		m_iLife--;
	}
}

bool Enemy::RectCheck(int x, int y, int Index)
{
	RECT tmpRect, CreateRect;
	CreateRect = { x, y , x + IMAGE_SIZE, y + IMAGE_SIZE };
	for (int i = 0; i < m_vEnemy.size(); i++)
	{
		if (Index == i)
			continue;
		if (IntersectRect(&tmpRect, &m_vEnemy[i].m_TankRect, &CreateRect))
			return false;
	}
	return true;
}

void Enemy::Draw(HDC hdc)
{
	//탱크그리기
	for (int i = 0; i < m_vEnemy.size(); i++)
	{
		if (m_vEnemy[i].m_iTankState == TANK_STATE_MOVE)
		{
			if (m_vEnemy[i].m_iIndex == IMAGE_ENEMY_DOWN1 || m_vEnemy[i].m_iIndex == IMAGE_ENEMY_LEFT1 ||
				m_vEnemy[i].m_iIndex == IMAGE_ENEMY_RIGHT1 || m_vEnemy[i].m_iIndex == IMAGE_ENEMY_UP1)
				m_vEnemy[i].m_iIndex++;
			else
				m_vEnemy[i].m_iIndex--;
		}
		Res_MG::GetInstance()->Draw(m_vEnemy[i].m_iIndex, hdc, m_vEnemy[i].m_Point, PUBLIC_SIZE);
		//탄그리기
		if (m_vEnemy[i].m_Bullet.m_iBulletState == BULLET_MOVE)
			Res_MG::GetInstance()->Draw(m_vEnemy[i].m_Bullet.m_iIndex, hdc, m_vEnemy[i].m_Bullet.m_Point, m_vEnemy[i].m_Bullet.m_Size);
		if (m_vEnemy[i].m_Bullet.m_iBulletState == BULLET_BREAK)
			BulletBoom(hdc, m_vEnemy[i]);
	}
	//라이프그리기
	GameInfoDraw(hdc);
}

void Enemy::Move(Map& Map)
{
	int AddX, AddY;
	int GaroGap, SaroGap;
	for (int i = 0; i < m_vEnemy.size(); i++)
	{
		//지역변수초기화
		AddX = 0;
		AddY = 0;
		m_vEnemy[i].m_iTankState = TANK_STATE_STOP;
		//플레이어추적
		if (clock() - m_vEnemy[i].m_iFindClock >= m_iLevelTime)//게임레벨에따라 찾는 빈도를 조절해서 난이도조정
		{
			GaroGap = m_vEnemy[i].m_Point.x - g_PlayerRect.left;
			SaroGap = m_vEnemy[i].m_Point.y - g_PlayerRect.top;
			if (abs(GaroGap) > abs(SaroGap))//플레이어와 현재 탱크의 가로차(절대값) 세로값(절대값) 비교
			{
				if (GaroGap > 0)//현재탱크가 플레이어보다 오른쪽에있을경우
					m_vEnemy[i].m_iMoveDirection = LEFT;
				else
					m_vEnemy[i].m_iMoveDirection = RIGHT;
			}
			else
			{
				if (SaroGap > 0)//현재탱크가 플레이어보다 아래있을경우
					m_vEnemy[i].m_iMoveDirection = UP;
				else
					m_vEnemy[i].m_iMoveDirection = DOWN;
			}
			m_vEnemy[i].m_iFindClock = clock();
			m_vEnemy[i].m_iMoveClock = clock();
		}
		//실제이동하기
		if (m_vEnemy[i].m_iMoveClock - clock() >= 0 && m_vEnemy[i].m_iMoveClock - clock() <= m_iMoveTime)
		{
			switch (m_vEnemy[i].m_iMoveDirection)
			{
			case LEFT:
				AddX = -m_iSpeed;
				if (m_vEnemy[i].m_iIndex != IMAGE_ENEMY_LEFT1 && m_vEnemy[i].m_iIndex != IMAGE_ENEMY_LEFT2)
					m_vEnemy[i].m_iIndex = IMAGE_ENEMY_LEFT1;
				m_vEnemy[i].m_iTankState = TANK_STATE_MOVE;
				break;
			case RIGHT:
				AddX = m_iSpeed;
				if (m_vEnemy[i].m_iIndex != IMAGE_ENEMY_RIGHT1 && m_vEnemy[i].m_iIndex != IMAGE_ENEMY_RIGHT2)
					m_vEnemy[i].m_iIndex = IMAGE_ENEMY_RIGHT1;
				m_vEnemy[i].m_iTankState = TANK_STATE_MOVE;
				break;
			case UP:
				AddY = -m_iSpeed;
				if (m_vEnemy[i].m_iIndex != IMAGE_ENEMY_UP1 && m_vEnemy[i].m_iIndex != IMAGE_ENEMY_UP2)
					m_vEnemy[i].m_iIndex = IMAGE_ENEMY_UP1;
				m_vEnemy[i].m_iTankState = TANK_STATE_MOVE;
				break;
			case DOWN:
				AddY = m_iSpeed;
				if (m_vEnemy[i].m_iIndex != IMAGE_ENEMY_DOWN1 && m_vEnemy[i].m_iIndex != IMAGE_ENEMY_DOWN2)
					m_vEnemy[i].m_iIndex = IMAGE_ENEMY_DOWN1;
				m_vEnemy[i].m_iTankState = TANK_STATE_MOVE;
				break;
			}
			//해당위치로 움직일수 있을시 움직이기
			if (Map.MoveCheck(m_vEnemy[i].m_Point.x + AddX, m_vEnemy[i].m_Point.y + AddY, ENEMY) != MOVE_FALSE &&
				RectCheck(m_vEnemy[i].m_Point.x + AddX, m_vEnemy[i].m_Point.y + AddY, i) == true)
			{
				m_vEnemy[i].m_Point.x += AddX;
				m_vEnemy[i].m_Point.y += AddY;
				m_vEnemy[i].m_TankRect = { m_vEnemy[i].m_Point.x , m_vEnemy[i].m_Point.y, m_vEnemy[i].m_Point.x + IMAGE_SIZE , m_vEnemy[i].m_Point.y + IMAGE_SIZE };
				m_vEnemy[i].m_AttackRange = { m_vEnemy[i].m_TankRect.left - (IMAGE_SIZE * m_iCreateMax), m_vEnemy[i].m_TankRect.top - (IMAGE_SIZE * m_iCreateMax),
				m_vEnemy[i].m_TankRect.right + (IMAGE_SIZE * m_iCreateMax), m_vEnemy[i].m_TankRect.bottom + (IMAGE_SIZE * m_iCreateMax) };
			}
			else//벽에막히는 등 못움직이는 상황
				m_vEnemy[i].m_iMoveDirection = rand() % 4;//방향재설정
		}
		else if (clock() >= m_vEnemy[i].m_iMoveClock)//일정시간 움직인 후 정지하는 시간 딜레이
			m_vEnemy[i].m_iMoveClock = clock() + m_iMoveTime + PAUSE_TIME;	
	}
	//충돌확인전역변수업데이트
	for (int i = 0; i < m_vEnemy.size(); i++)
		g_arrEnemyRect[i] = m_vEnemy[i].m_TankRect;
}

void Enemy::Attack(Map& Map)
{
	for (int i = 0; i < m_vEnemy.size(); i++)
	{
		if (m_vEnemy[i].m_Bullet.m_iBulletState == BULLET_NONE)
		{
			//플레이어가 공격범위 안 일때 일정확률로 공격 진행
			RECT tmpRect;
			if (IntersectRect(&tmpRect, &m_vEnemy[i].m_AttackRange, &g_PlayerRect) && rand() % 10 + 1 <= (m_iMoveTime / 1000))
			{
				m_vEnemy[i].m_Bullet.m_iBulletState = BULLET_MOVE;
				m_vEnemy[i].m_Bullet.m_Point = { m_vEnemy[i].m_Point.x + HALF - m_vEnemy[i].m_Bullet.m_Size.cx / 2, m_vEnemy[i].m_Point.y + HALF - m_vEnemy[i].m_Bullet.m_Size.cy / 2 };
				m_vEnemy[i].m_iAttackDirection = m_vEnemy[i].m_iMoveDirection;
			}
		}
		if (m_vEnemy[i].m_Bullet.m_iBulletState == BULLET_MOVE)
		{
			switch (m_vEnemy[i].m_iAttackDirection)
			{
			case UP:
				m_vEnemy[i].m_Bullet.m_Point.y -= BULLET_SPEED;
				break;
			case DOWN:
				m_vEnemy[i].m_Bullet.m_Point.y += BULLET_SPEED;
				break;
			case LEFT:
				m_vEnemy[i].m_Bullet.m_Point.x -= BULLET_SPEED;
				break;
			case RIGHT:
				m_vEnemy[i].m_Bullet.m_Point.x += BULLET_SPEED;
				break;
			}
			m_vEnemy[i].m_Bullet.m_Rect = { m_vEnemy[i].m_Bullet.m_Point.x, m_vEnemy[i].m_Bullet.m_Point.y,
				m_vEnemy[i].m_Bullet.m_Point.x + m_vEnemy[i].m_Bullet.m_Size.cx, m_vEnemy[i].m_Bullet.m_Point.y + m_vEnemy[i].m_Bullet.m_Size.cy };
		}
		//공격충돌체크
		RECT tmpRect;
		if (m_vEnemy[i].m_Bullet.m_iBulletState != BULLET_NONE)
		{
			//플레이어 충돌확인
			if (IntersectRect(&tmpRect, &m_vEnemy[i].m_Bullet.m_Rect, &g_PlayerRect) == true)
			{
				g_PlayerRect = { DIE, DIE, DIE, DIE };
				m_vEnemy[i].m_Bullet.m_iBulletState = BULLET_BREAK;
			}
			//벽 충돌확인
			if(Map.BreakCheck(m_vEnemy[i].m_Bullet.m_Point.x, m_vEnemy[i].m_Bullet.m_Point.y, m_vEnemy[i].m_iAttackDirection) == true)
				m_vEnemy[i].m_Bullet.m_iBulletState = BULLET_BREAK;
		}
	}
}

void Enemy::Die()
{
	if (Index != EMPTY)
	{
		m_vEnemy.erase(m_vEnemy.begin() + Index);
		Index = EMPTY;
		m_iCreateClock = clock();
		//전역변수초기화
		memset(g_arrEnemyRect, DIE, sizeof(RECT) * 3);
		//전역변수업데이트
		for (int i = 0; i < m_vEnemy.size(); i++)
			g_arrEnemyRect[i] = m_vEnemy[i].m_TankRect;
	}
}

bool Enemy::GameCheck(int& Level)
{
	if (m_iLife == 0 && m_vEnemy.size() == 0)
	{
		Level++;
		ResetTank(Level);
		return true;
	}
	return false;
}

Enemy::~Enemy()
{
}
//적