
#include "stdafx.h"
#include "rigidstatic.h"

using namespace phys;
using namespace physx;


cRigidStatic::cRigidStatic()
	: cRigidActor()
{
}

cRigidStatic::~cRigidStatic()
{
	Clear();
}


void cRigidStatic::Clear()
{
	__super::Clear();

}
