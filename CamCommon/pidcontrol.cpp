
#include "stdafx.h"
#include "pidcontrol.h"


using namespace cv;

cPIDControl::cPIDControl() :
	m_originalPos(Point2f(0,0))
	, m_Pos(Point2f(0, 0))
	, m_oldDiff(Point2f(0,0))
	, m_Kp(1)
	, m_Ki(0)
	, m_Kd(0)
{
}

cPIDControl::~cPIDControl()
{
}


Point2f cPIDControl::Update(const Point2f &pos)
{
	m_originalPos = pos;

	const Point2f diff = pos - m_Pos;
	const Point2f eDiff = diff - m_oldDiff;
	m_oldDiff = diff;


	const float dx = (diff.x * m_Kp);
	const float dy = (diff.y * m_Kp);
	m_Pos.x += dx;
	m_Pos.y += dy;


	const float ddx = (eDiff.x * m_Kd);
	const float ddy = (eDiff.y * m_Kd);
	m_Pos.x += ddx;
	m_Pos.y += ddy;

	return m_Pos;
}

