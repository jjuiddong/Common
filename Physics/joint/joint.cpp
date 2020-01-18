
#include "stdafx.h"
#include "joint.h"

using namespace phys;
using namespace physx;


cJoint::cJoint()
	: m_joint(nullptr)
	, m_fixedJoint(nullptr)
	, m_sphericalJoint(nullptr)
	, m_type(eType::None)
{
}

cJoint::~cJoint()
{
	Clear();
}


// worldTfm0, worldTfm1 is not same PxFixedJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor0 current world transform
bool cJoint::CreateFixedJoint(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0
	, cRigidActor *actor1, const Transform &worldTfm1)
{
	RETV(!physics.m_physics, false);

	const Vector3 center = (worldTfm0.pos + worldTfm1.pos) * 0.5f;
	const Vector3 p0 = center - worldTfm0.pos;
	const Vector3 p1 = center - worldTfm1.pos;
	const Quaternion q0 = worldTfm0.rot.Inverse();
	const Quaternion q1 = worldTfm1.rot.Inverse();
	const PxTransform localFrame0 = PxTransform(*(PxQuat*)&q0) * PxTransform(*(PxVec3*)&p0);
	const PxTransform localFrame1 = PxTransform(*(PxQuat*)&q1) * PxTransform(*(PxVec3*)&p1);

	PxFixedJoint *joint = PxFixedJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	m_type = eType::Fixed;
	m_fixedJoint = joint;
	return true;
}


// worldTfm0, worldTfm1 is not same PxSphericalJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor0 current world transform
bool cJoint::CreateSphericalJoint(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0
	, cRigidActor *actor1, const Transform &worldTfm1)
{
	RETV(!physics.m_physics, false);

	const Vector3 center = (worldTfm0.pos + worldTfm1.pos) * 0.5f;
	const Vector3 p0 = center - worldTfm0.pos;
	const Vector3 p1 = center - worldTfm1.pos;
	const Quaternion q0 = worldTfm0.rot.Inverse();
	const Quaternion q1 = worldTfm1.rot.Inverse();
	const PxTransform localFrame0 = PxTransform(*(PxQuat*)&q0) * PxTransform(*(PxVec3*)&p0);
	const PxTransform localFrame1 = PxTransform(*(PxQuat*)&q1) * PxTransform(*(PxVec3*)&p1);

	PxSphericalJoint *joint = PxSphericalJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	m_type = eType::Fixed;
	m_sphericalJoint = joint;

	return true;
}


void cJoint::Clear()
{
	PHY_SAFE_RELEASE(m_joint);
	m_fixedJoint = nullptr;
	m_sphericalJoint = nullptr;
}
