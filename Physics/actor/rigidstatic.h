//
// 2020-01-18, jjuiddong
// rigid static actor
//		- PhysX PxRigidStatic wrapping class
//		- PhysX class hierarchy
//			- https://gameworksdocs.nvidia.com/PhysX/3.4/PhysXAPI/files/classPxRigidActor.html
//
#pragma once

#include "rigidactor.h"


namespace phys
{

	class cRigidStatic : public cRigidActor
	{
	public:
		cRigidStatic();
		virtual ~cRigidStatic();

		void Clear();


	public:
	};

}
