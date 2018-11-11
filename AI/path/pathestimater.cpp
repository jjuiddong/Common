
#include "stdafx.h"
#include "pathestimater.h"


using namespace ai;


cPathEstimater::cPathEstimater()
	: m_velocity(1.f)
	, m_angularVelocity(1.f)
{
}

cPathEstimater::~cPathEstimater()
{
}


// 경로 정보를 토대로, 이동 경로에 따른 시간, 위치를 예측한다.
bool cPathEstimater::Estimate(const cPathFinder2D &pathFinder
	, const cPathFinder2D::ppath &posPath
	, const cPathFinder2D::epath &edgePath)
{




	return true;
}
