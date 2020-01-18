
#include "stdafx.h"
#include "rigiddynamic.h"

using namespace phys;
using namespace physx;


cRigidDynamic::cRigidDynamic()
	: cRigidActor()
{
}

cRigidDynamic::~cRigidDynamic()
{
	Clear();
}


void cRigidDynamic::Clear()
{
	__super::Clear();
}
