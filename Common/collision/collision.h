//
// 2018-10-06, jjuiddong
// collision global include
//
#pragma once


namespace common
{

	// 경계박스 min max 값.
	// AABB
	struct sMinMax
	{
		Vector3 _min;
		Vector3 _max;
		sMinMax() : _min(Vector3::Max), _max(Vector3::Min) {}

		void Update(const Vector3 &pos)
		{
			if (_min.x > pos.x)
				_min.x = pos.x;
			if (_min.y > pos.y)
				_min.y = pos.y;
			if (_min.z > pos.z)
				_min.z = pos.z;

			if (_max.x < pos.x)
				_max.x = pos.x;
			if (_max.y < pos.y)
				_max.y = pos.y;
			if (_max.z < pos.z)
				_max.z = pos.z;
		}


		bool IsOk()
		{ // min, max 초기값 그대로이면 false를 리턴한다.
			// 혹은 max 가 min 보다 작으면 false를 리턴한다.
			if ((_max.x < _min.x) || (_max.y < _min.y) || (_max.z < _min.z))
				return false;
			return true;
		}
	};

}



#include "collisionobj.h"
#include "boundingbox.h"
#include "boundinghemisphere.h"
#include "boundingplane.h"
#include "boundingsphere.h"
