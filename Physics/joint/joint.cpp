
#include "stdafx.h"
#include "joint.h"

using namespace phys;
using namespace physx;


cJoint::cJoint()
	: m_joint(nullptr)
	, m_type(eType::None)
{
}

cJoint::~cJoint()
{
	Clear();
}


void cJoint::Clear()
{
	PHY_SAFE_RELEASE(m_joint);
}
