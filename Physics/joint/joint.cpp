
#include "stdafx.h"
#include "joint.h"

using namespace phys;
using namespace physx;


cJoint::cJoint()
	: m_joint(nullptr)
	, m_fixedJoint(nullptr)
	, m_sphericalJoint(nullptr)
	, m_revoluteJoint(nullptr)
	, m_type(eType::None)
	, m_actor0(nullptr)
	, m_actor1(nullptr)
{
}

cJoint::~cJoint()
{
	Clear();
}


// worldTfm0, worldTfm1 is not same PxFixedJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor0 current world transform
bool cJoint::CreateFixed(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0
	, cRigidActor *actor1, const Transform &worldTfm1)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	GetLocalFrame(worldTfm0, worldTfm1, Vector3::Zeroes
		, localFrame0, localFrame1);

	PxFixedJoint *joint = PxFixedJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	m_type = eType::Fixed;
	m_joint = joint;
	m_fixedJoint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	return true;
}


// worldTfm0, worldTfm1 is not same PxSphericalJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor0 current world transform
bool cJoint::CreateSpherical(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0
	, cRigidActor *actor1, const Transform &worldTfm1)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	GetLocalFrame(worldTfm0, worldTfm1, Vector3::Zeroes
		, localFrame0, localFrame1);

	PxSphericalJoint *joint = PxSphericalJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	m_type = eType::Spherical;
	m_joint = joint;
	m_sphericalJoint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	return true;
}


// worldTfm0, worldTfm1 is not same PxRevoluteJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor0 current world transform
bool cJoint::CreateRevolute(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0
	, cRigidActor *actor1, const Transform &worldTfm1
	, const Vector3 &revoluteAxis)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	GetLocalFrame(worldTfm0, worldTfm1, revoluteAxis
		, localFrame0, localFrame1);

	PxRevoluteJoint *joint = PxRevoluteJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	m_type = eType::Revolute;
	m_joint = joint;
	m_revoluteJoint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	return true;
}


// calc localFrame for PxJoint~ Function seriese
// worldTm0 : actor0 world transform
// worldTm1 : actor1 world transform
// revoluteAxis : revolution Axis
//			      if ZeroVector, ignore revolute axis
// out0 : return actor0 localFrame
// out1 : return actor1 localFrame
void cJoint::GetLocalFrame(const Transform &worldTm0, const Transform &worldTm1
	, const Vector3 &revoluteAxis
	, OUT physx::PxTransform &out0, OUT physx::PxTransform &out1)
{
	Transform tfm0 = worldTm0;
	Transform tfm1 = worldTm1;

	const Vector3 center = (tfm0.pos + tfm1.pos) * 0.5f;
	Vector3 p0 = center - tfm0.pos;
	Vector3 p1 = center - tfm1.pos;
	Quaternion q0 = tfm0.rot.Inverse();
	Quaternion q1 = tfm1.rot.Inverse();

	if (revoluteAxis != Vector3::Zeroes)
	{
		Quaternion rot(revoluteAxis, Vector3(1, 0, 0));
		tfm0.rot *= rot;
		tfm1.rot *= rot;

		p0 = (tfm0.pos - center) * rot + center;
		p1 = (tfm1.pos - center) * rot + center;
		p0 = center - p0;
		p1 = center - p1;
		q0 = tfm0.rot.Inverse();
		q1 = tfm1.rot.Inverse();
	}

	const PxTransform localFrame0 = PxTransform(*(PxQuat*)&q0) * PxTransform(*(PxVec3*)&p0);
	const PxTransform localFrame1 = PxTransform(*(PxQuat*)&q1) * PxTransform(*(PxVec3*)&p1);

	out0 = localFrame0;
	out1 = localFrame1;
}


void cJoint::Clear()
{
	PHY_SAFE_RELEASE(m_joint);
	m_fixedJoint = nullptr;
	m_sphericalJoint = nullptr;
	m_revoluteJoint = nullptr;
	m_actor0 = nullptr;
	m_actor1 = nullptr;
}
