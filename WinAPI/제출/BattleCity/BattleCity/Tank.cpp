#include "Tank.h"

Tank::Tank()
{
}

RECT Tank::ResetTank(int UnitType, POINT pt)
{
	//플레이어타입에따른 인덱스 수정
	int AddIndex = 0;
	if (UnitType == ENEMY)
		AddIndex = -8;
	m_iUnitType = UnitType;
	//탱크그림설정
	for (int i = DIRECTION_START; i < DIRECTION_MAX; i++)
		m_arriIndex[i] = IMAGE_PLAYER1_DOWN1 + AddIndex + (i * 2);
	for (int arrIndex = 0, ImageIndex = IMAGE_PLAYER2_DOWN; ImageIndex <= IMAGE_PLAYER2_UP; arrIndex++, ImageIndex++)
		m_arriItemIndex[arrIndex] = ImageIndex;
	//탱크위치설정
	m_TankPt = pt;
	//탱크Rect설정
	m_TankRect = { m_TankPt.x - AIR, m_TankPt.y - AIR, m_TankPt.x + IMAGE_SIZE + AIR, m_TankPt.y + IMAGE_SIZE + AIR };
	//탱크 초기상태설정
	m_iTankState = TANK_STOP;
	//탱크 공격범위 설정
	m_AttackRange = { m_TankPt.x - SEARCH, m_TankPt.y - SEARCH, m_TankPt.x + SEARCH, m_TankPt.y + SEARCH };
	//초기방향설정
	m_iMoveDir = DIRECTION_UP;
	if(UnitType == ENEMY)
		m_iMoveDir = DIRECTION_DOWN;
	//탱크속도설정
	m_iSpeed = MOVE_SPEED;
	//탱크아이템설정
	m_iItem = EMPTY;
	//탄 설정
	m_Bullet.m_iBulletState = BULLET_NONE;
	m_Bullet.m_iIndex = IMAGE_BULLET;
	m_Bullet.m_Point = { m_TankPt.x + BULLET_PT_SET, m_TankPt.y + BULLET_PT_SET };
	m_Bullet.m_Rect = { m_Bullet.m_Point.x, m_Bullet.m_Point.y, m_Bullet.m_Point.x + BULLET_SIZE, m_Bullet.m_Point.y + BULLET_SIZE };
	m_Bullet.m_Size = { BULLET_SIZE, BULLET_SIZE };
	m_iDelay = 0;
	//시간정보들 설정
	m_iFindClock = clock();
	m_iMoveClock = clock();
	//탱크Rect반환
	return m_TankRect;
}

void Tank::DrawTank(HDC hdc)
{
	//평상시 탱크그리기
	if (m_iTankState == TANK_MOVE)
	{
		if (m_arriIndex[m_iMoveDir] % 2 == 0)
			m_arriIndex[m_iMoveDir]--;
		else
			m_arriIndex[m_iMoveDir]++;
	}
	Res_MG::GetInstance()->Draw(m_arriIndex[m_iMoveDir], hdc, m_TankPt, PUBLIC_SIZE);
	//아이템상태그리기
	if (m_iItem != EMPTY)
	{
		switch (m_iItem)
		{
		case TANK_ITEM_SPEEDUP:
			Res_MG::GetInstance()->Draw(m_arriItemIndex[m_iMoveDir], hdc, m_TankPt, PUBLIC_SIZE);
			break;
		case TANK_ITEM_SHILED:
			m_iItem = IMAGE_SHIELD1;
			Res_MG::GetInstance()->Draw(m_iItem, hdc, m_TankPt, PUBLIC_SIZE);
			break;
		case IMAGE_SHIELD1:
			m_iItem = IMAGE_SHIELD2;
			Res_MG::GetInstance()->Draw(m_iItem, hdc, m_TankPt, PUBLIC_SIZE);
			break;
		case IMAGE_SHIELD2:
			m_iItem = IMAGE_SHIELD1;
			Res_MG::GetInstance()->Draw(m_iItem, hdc, m_TankPt, PUBLIC_SIZE);
			break;
		}
	}

	//총알그리기
	if (m_Bullet.m_iBulletState == BULLET_MOVE)
		Res_MG::GetInstance()->Draw(m_Bullet.m_iIndex, hdc, m_Bullet.m_Point, m_Bullet.m_Size);
	if (m_Bullet.m_iBulletState == BULLET_BREAK)
		BulletBoom(hdc);

}

RECT Tank::Move(Map Map, RECT* Rect)
{
	//공격상태설정
	RECT tmpRect;
	if (IntersectRect(&tmpRect, &m_AttackRange, &Rect[0]) == true && rand() % 3 == 0)
		SetBullet();
	//움직이기(플레이어 추적)
	if (clock() - m_iFindClock >= PAUSE_TIME * 2)
	{
		int GapX, GapY;
		GapX = m_TankRect.left - Rect[0].left;
		GapY = m_TankRect.top - Rect[0].top;
		if (abs(GapX) > abs(GapY))//플레이어와 현재 탱크의 가로차(절대값) 세로값(절대값) 비교
		{
			if (GapX > 0)//현재탱크가 플레이어보다 오른쪽에있을경우
				m_iMoveDir = DIRECTION_LEFT;
			else
				m_iMoveDir = DIRECTION_RIGHT;
		}
		else
		{
			if (GapY > 0)//현재탱크가 플레이어보다 아래있을경우
				m_iMoveDir = DIRECTION_UP;
			else
				m_iMoveDir = DIRECTION_DOWN;
		}
		m_iFindClock = clock();
	}
	if (m_iMoveClock - clock() >= 0 && m_iMoveClock - clock() <= MOVE_TIME)
		return Move(m_iMoveDir, Map, Rect);
	else if (clock() >= m_iMoveClock)//일정시간 움직인 후 정지하는 시간 딜레이
	{
		m_iMoveClock = clock() + MOVE_TIME + PAUSE_TIME;
		m_iTankState = TANK_STOP;
	}
	m_AttackRange = { m_TankPt.x - SEARCH, m_TankPt.y - SEARCH, m_TankPt.x + SEARCH, m_TankPt.y + SEARCH };
	return m_TankRect;
}

RECT Tank::Move(int Direction, Map& Map, RECT* Rect)
{
	int AddX = 0, AddY = 0;
	if (Direction != EMPTY)//방향이 존재할시
	{
		m_iTankState = TANK_MOVE;
		m_iMoveDir = Direction;
		switch (m_iMoveDir)
		{
		case DIRECTION_DOWN:
			AddY = m_iSpeed;
			break;
		case DIRECTION_LEFT:
			AddX = -m_iSpeed;
			break;
		case DIRECTION_RIGHT:
			AddX = m_iSpeed;
			break;
		case DIRECTION_UP:
			AddY = -m_iSpeed;
			break;
		}
		m_AttackRange = { m_TankPt.x - SEARCH, m_TankPt.y - SEARCH, m_TankPt.x + SEARCH, m_TankPt.y + SEARCH };
		//탱크간충돌확인
		int Count = 0;
		bool Check = true;
		m_PublicRect = { m_TankPt.x + AddX, m_TankPt.y + AddY, m_TankPt.x + AddX + IMAGE_SIZE, m_TankPt.y + AddY + IMAGE_SIZE };
		for (int i = 0; i < 4; i++)
		{
			if (CollideCheck(m_PublicRect, Rect[i]))
				Count++;
			if (Count >= 2)
			{
				if (m_iUnitType == ENEMY)//적일경우 못움직이면 방향 재설정
					m_iMoveDir = rand() % 4;
				Check = false;
			}
		}
		//맵간충돌확인, 탱크간 충돌확인
		int Result = Map.MoveCheck(m_TankPt.x + AddX, m_TankPt.y + AddY, m_iUnitType);
		if (Result != MOVE_FALSE && Check == true)
		{
			m_TankPt.x += AddX;
			m_TankPt.y += AddY;
			if (Result == MOVE_ITEM && m_iUnitType == PLAYER)
			{
				if (rand() % 2 == 0)
					m_iItem = TANK_ITEM_SHILED;
				else
				{
					m_iItem = TANK_ITEM_SPEEDUP;
					m_iSpeed += 10;
				}
			}
		}
		else if (m_iUnitType == ENEMY)//적일경우 못움직이면 방향 재설정
			m_iMoveDir = rand() % 4;
	}
	else//방향이없음
		m_iTankState = TANK_STOP;
	m_AttackRange = { m_TankPt.x - SEARCH, m_TankPt.y - SEARCH, m_TankPt.x + SEARCH, m_TankPt.y + SEARCH };
	m_TankRect = { m_TankPt.x - AIR, m_TankPt.y - AIR ,m_TankPt.x + IMAGE_SIZE + AIR, m_TankPt.y + IMAGE_SIZE + AIR };
	return m_TankRect;
}

void Tank::SetBullet()
{
	if (m_Bullet.m_iBulletState == BULLET_NONE)
	{
		m_Bullet.m_iBulletState = BULLET_MOVE;
		m_Bullet.m_Point = { m_TankPt.x + BULLET_PT_SET, m_TankPt.y + BULLET_PT_SET };
		m_iAttackDir = m_iMoveDir;
	}
}

RECT Tank::Attack(Map& Map)
{
	if (m_Bullet.m_iBulletState == BULLET_MOVE)
	{
		switch (m_iAttackDir)
		{
		case DIRECTION_UP:
			m_Bullet.m_Point.y -= BULLET_SPEED;
			break;
		case DIRECTION_DOWN:
			m_Bullet.m_Point.y += BULLET_SPEED;
			break;
		case DIRECTION_LEFT:
			m_Bullet.m_Point.x -= BULLET_SPEED;
			break;
		case DIRECTION_RIGHT:
			m_Bullet.m_Point.x += BULLET_SPEED;
			break;
		}
		m_Bullet.m_Rect = { m_Bullet.m_Point.x, m_Bullet.m_Point.y, m_Bullet.m_Point.x + BULLET_SIZE, m_Bullet.m_Point.y + BULLET_SIZE };
	}
	//벽충돌확인
	if (Map.BreakCheck(m_Bullet.m_Point.x, m_Bullet.m_Point.y, m_iAttackDir) == true)
		m_Bullet.m_iBulletState = BULLET_BREAK;
	return m_Bullet.m_Rect;
}

void Tank::BulletBoom(HDC hdc)
{
	POINT tmpPoint = { m_Bullet.m_Point.x - BULLET_PT_SET , m_Bullet.m_Point.y - BULLET_PT_SET};
	m_Bullet.m_Rect = { DIE, DIE, DIE, DIE };
	int Index = IMAGE_BULLET_BOOM1 + m_iDelay;
	Res_MG::GetInstance()->Draw(Index, hdc, tmpPoint, PUBLIC_SIZE);
	m_iDelay++;
	if (m_iDelay == 3)
	{
		m_Bullet.m_iBulletState = BULLET_NONE;
		m_Bullet.m_Point = { m_TankPt.x + BULLET_PT_SET, m_TankPt.y + BULLET_PT_SET };
		m_Bullet.m_Rect = { m_Bullet.m_Point.x, m_Bullet.m_Point.y, m_Bullet.m_Point.x + BULLET_SIZE, m_Bullet.m_Point.y + BULLET_SIZE };
		m_iDelay = 0;
	}
}

bool Tank::CollideCheck(RECT PlayerRect, RECT Rect)
{
	RECT tmpRect;
	if (PlayerRect.left == Rect.left)
	{
		if (IntersectRect(&tmpRect, &m_TankRect, &Rect))
			return true;
	}
	else
	{
		if (IntersectRect(&tmpRect, &PlayerRect, &Rect))
			return true;;
	}
	return false;
}

Tank::~Tank()
{
}