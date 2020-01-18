//
// 2020-01-18, jjuiddong
// rigid dynamic actor
//		- PhysX PxRigidDynamic wrapping class
//		- PhysX class hierarchy
//			- https://gameworksdocs.nvidia.com/PhysX/3.4/PhysXAPI/files/classPxRigidActor.html
//
#pragma once

#include "rigidactor.h"


namespace phys
{

	class cRigidDynamic : public cRigidActor
	{
	public:
		cRigidDynamic();
		virtual ~cRigidDynamic();

		void Clear();


	public:
	};

}
