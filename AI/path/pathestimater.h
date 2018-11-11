//
// 2018-11-11, jjuiddong
// cPathFinder2D path time estimate
//
#pragma once

#include "pathfinder2d.h"

namespace ai
{

	class cPathEstimater
	{
	public:
		cPathEstimater();
		virtual ~cPathEstimater();

		bool Estimate(const cPathFinder2D &pathFinder
			, const cPathFinder2D::ppath &posPath
			, const cPathFinder2D::epath &edgePath);


	public:
		float m_velocity; // m/s
		float m_angularVelocity; // radian/s
	};

}
