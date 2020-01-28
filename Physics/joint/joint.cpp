
#include "stdafx.h"
#include "joint.h"

using namespace phys;
using namespace physx;


cJoint::cJoint()
	: m_id(common::GenerateId())
	, m_joint(nullptr)
	, m_type(eJointType::None)
	, m_actor0(nullptr)
	, m_actor1(nullptr)
	, m_referenceMode(false)
	, m_revoluteAxis(Vector3(1,0,0))
	, m_breakForce(2000.f)
	//, m_breakForce(0.f)
	, m_isBroken(false)
	, m_isCycleDrive(false)
	, m_incT(0.f)
	, m_incAccelT(0.f)
	, m_cyclePeriod(3.f)
	, m_maxDriveVelocity(0.f)
	, m_curVelocity(0.f)
	, m_cycleDriveAccel(0.f)
	, m_toggleDir(true)
{
}

cJoint::~cJoint()
{
	Clear();
}


// reference mode, no remove actor, joint object
// to show joint information with ui
bool cJoint::CreateReferenceMode()
{
	m_referenceMode = true;
	return true;
}


// worldTfm0, worldTfm1 is not same PxFixedJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor1 current world transform
bool cJoint::CreateFixed(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0
	, cRigidActor *actor1, const Transform &worldTfm1)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (worldTfm0.pos + worldTfm1.pos) / 2.f;
	GetLocalFrame(worldTfm0, worldTfm1, jointPos
		, Vector3::Zeroes, localFrame0, localFrame1);

	PxFixedJoint *joint = PxFixedJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	DefaultJointConfiguration(joint);

	m_type = eJointType::Fixed;
	m_joint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	m_origPos = jointPos;
	m_actorLocal0 = worldTfm0;
	m_actorLocal1 = worldTfm1;
	return true;
}


// worldTfm0, worldTfm1 is not same PxSphericalJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor1 current world transform
bool cJoint::CreateSpherical(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0
	, cRigidActor *actor1, const Transform &worldTfm1)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (worldTfm0.pos + worldTfm1.pos) / 2.f;
	GetLocalFrame(worldTfm0, worldTfm1, jointPos
		, Vector3::Zeroes, localFrame0, localFrame1);

	PxSphericalJoint *joint = PxSphericalJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	DefaultJointConfiguration(joint);

	m_type = eJointType::Spherical;
	m_joint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	m_origPos = jointPos;
	m_actorLocal0 = worldTfm0;
	m_actorLocal1 = worldTfm1;
	return true;
}


// worldTfm0, worldTfm1 is not same PxRevoluteJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor1 current world transform
bool cJoint::CreateRevolute(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
	, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1
	, const Vector3 &revoluteAxis)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (pivot0 + pivot1) / 2.f;
	GetLocalFrame(worldTfm0, worldTfm1, jointPos
		, revoluteAxis, localFrame0, localFrame1);

	PxRevoluteJoint *joint = PxRevoluteJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	DefaultJointConfiguration(joint);

	m_type = eJointType::Revolute;
	m_joint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	m_revoluteAxis = revoluteAxis;
	m_origPos = jointPos;
	m_rotRevolute.SetRotationArc(Vector3(1,0,0), revoluteAxis);
	m_revoluteAxisLen = pivot1.Distance(pivot0);
	m_actorLocal0 = worldTfm0;
	m_actorLocal1 = worldTfm1;
	// world -> local space
	m_pivots[0].dir = (pivot0 - worldTfm0.pos).Normal() * worldTfm0.rot.Inverse();
	m_pivots[0].len = (pivot0 - worldTfm0.pos).Length();
	m_pivots[1].dir = (pivot1 - worldTfm1.pos).Normal() * worldTfm1.rot.Inverse();
	m_pivots[1].len = (pivot1 - worldTfm1.pos).Length();
	return true;
}


// worldTfm0, worldTfm1 is not same PxRevoluteJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor1 current world transform
bool cJoint::CreatePrismatic(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
	, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1
	, const Vector3 &revoluteAxis)
{
	RETV(!physics.m_physics, false);

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (pivot0 + pivot1) / 2.f;
	GetLocalFrame(worldTfm0, worldTfm1, jointPos
		, revoluteAxis, localFrame0, localFrame1);

	PxPrismaticJoint *joint = PxPrismaticJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	DefaultJointConfiguration(joint);

	m_type = eJointType::Prismatic;
	m_joint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	m_revoluteAxis = revoluteAxis;
	m_origPos = jointPos;
	m_rotRevolute.SetRotationArc(Vector3(1, 0, 0), revoluteAxis);
	m_revoluteAxisLen = pivot1.Distance(pivot0);
	m_actorLocal0 = worldTfm0;
	m_actorLocal1 = worldTfm1;
	// world -> local space
	m_pivots[0].dir = (pivot0 - worldTfm0.pos).Normal() * worldTfm0.rot.Inverse();
	m_pivots[0].len = (pivot0 - worldTfm0.pos).Length();
	m_pivots[1].dir = (pivot1 - worldTfm1.pos).Normal() * worldTfm1.rot.Inverse();
	m_pivots[1].len = (pivot1 - worldTfm1.pos).Length();
	return true;
}


// worldTfm0, worldTfm1 is not same PxRevoluteJointCreate() argument localFrame
// worldTfm0 : actor0 current world transform
// worldTfm1 : actor1 current world transform
bool cJoint::CreateDistance(cPhysicsEngine &physics
	, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
	, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1)
{
	RETV(!physics.m_physics, false);

	const Vector3 revoluteAxis(1, 0, 0);

	PxTransform localFrame0, localFrame1;
	const Vector3 jointPos = (pivot0 + pivot1) / 2.f;
	GetLocalFrame(worldTfm0, worldTfm1, jointPos
		, revoluteAxis, localFrame0, localFrame1);

	PxDistanceJoint *joint = PxDistanceJointCreate(*physics.m_physics
		, actor0->m_actor, localFrame0
		, actor1->m_actor, localFrame1);

	DefaultJointConfiguration(joint);

	m_type = eJointType::Distance;
	m_joint = joint;
	m_actor0 = actor0;
	m_actor1 = actor1;
	actor0->AddJoint(this);
	actor1->AddJoint(this);
	m_revoluteAxis = revoluteAxis;
	m_origPos = jointPos;
	m_rotRevolute.SetRotationArc(Vector3(1, 0, 0), revoluteAxis);
	m_revoluteAxisLen = pivot1.Distance(pivot0);
	m_actorLocal0 = worldTfm0;
	m_actorLocal1 = worldTfm1;
	// world -> local space
	m_pivots[0].dir = (pivot0 - worldTfm0.pos).Normal() * worldTfm0.rot.Inverse();
	m_pivots[0].len = (pivot0 - worldTfm0.pos).Length();
	m_pivots[1].dir = (pivot1 - worldTfm1.pos).Normal() * worldTfm1.rot.Inverse();
	m_pivots[1].len = (pivot1 - worldTfm1.pos).Length();
	return true;
}


// update joint drive velocity
bool cJoint::Update(const float deltaSeconds)
{
	RETV(!m_joint, false);
	RETV(!m_isCycleDrive, false);
	RETV(m_isBroken, false);
	RETV(m_type != eJointType::Revolute, false);

	// broken joint?
	const bool broken = (m_joint->getConstraintFlags() & physx::PxConstraintFlag::eBROKEN);
	if (broken)
	{
		m_isBroken = true;
		return true;
	}

	m_incT += deltaSeconds;
	if (m_cyclePeriod < m_incT)
	{
		// change velocity direction
		m_incT = 0.f;

		const float velocity = GetDriveVelocity();
		if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
			p->setDriveVelocity(-velocity);

		//m_curVelocity = 0.f;
		//m_incAccelT = 0.f;
		//if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
		//	p->setDriveVelocity(0.f);
		m_actor0->WakeUp();
		m_actor1->WakeUp();

		m_toggleDir = !m_toggleDir; // toggle rotation direction +/-
	}
	//else
	//{
	//	m_incAccelT += deltaSeconds;

	//	if ((m_incAccelT > 0.1f) && (abs(m_maxDriveVelocity - m_curVelocity) > 0.1f))
	//	{
	//		m_curVelocity += (m_cycleDriveAccel * m_incAccelT);
	//		m_curVelocity = min(m_maxDriveVelocity, m_curVelocity);
	//		if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
	//			p->setDriveVelocity(m_toggleDir ? m_curVelocity : -m_curVelocity);

	//		m_incAccelT = 0.f;
	//	}
	//}

	return true;
}


// return pivot world transform
// actorIndex: 0=actor0, 1=actor1
Transform cJoint::GetPivotWorldTransform(const int actorIndex)
{
	if ((actorIndex != 0) && (actorIndex != 1))
	{
		assert(0);
		return Transform();
	}

	const Transform tfm = (actorIndex == 0) ? m_actorLocal0 : m_actorLocal1;

	Transform ret;
	ret.pos = GetPivotPos(actorIndex);
	ret.scale = tfm.scale;
	ret.rot = tfm.rot;
	return ret;
}


// return pivot pos
Vector3 cJoint::GetPivotPos(const int actorIndex)
{
	if ((actorIndex != 0) && (actorIndex != 1))
		return Vector3::Zeroes;

	Vector3 pivotPos;
	const Transform tfm = (actorIndex == 0) ? m_actorLocal0 : m_actorLocal1;

	if (m_pivots[actorIndex].len != 0.f)
	{
		const Vector3 localPos = m_pivots[actorIndex].dir * tfm.rot
			* m_pivots[actorIndex].len;
		pivotPos = tfm.pos + localPos;
	}
	else
	{
		pivotPos = tfm.pos;
	}
	return pivotPos;
}


// actorIndex: 0=actor0, 1=actor1
// pos : pivot global pos
void cJoint::SetPivotPos(const int actorIndex, const Vector3 &pos)
{
	if ((actorIndex != 0) && (actorIndex != 1))
	{
		assert(0);
		return;
	}

	// change local coordinate system
	const Transform tfm = (actorIndex == 0) ? m_actorLocal0 : m_actorLocal1;
	const Vector3 dir = (pos - tfm.pos);
	m_pivots[actorIndex].dir = dir.Normal() * tfm.rot.Inverse();
	m_pivots[actorIndex].len = dir.Length();
}


// Spherical joint
// enable/disable cone limit
bool cJoint::EnableConeLimit(const bool enable)
{
	return SetSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, enable);
}


// Spherical joint
// is enable cone limit?
bool cJoint::IsConeLimit()
{
	RETV(!m_joint, false);

	if (PxSphericalJoint *p = m_joint->is<PxSphericalJoint>())
	{
		const PxSphericalJointFlags flags = p->getSphericalJointFlags();
		const bool isLimit = flags.isSet(PxSphericalJointFlag::eLIMIT_ENABLED);
		return isLimit;
	}
	return false;
}


// set spherical joint limit configuration
bool cJoint::SetConeLimit(const physx::PxJointLimitCone &config)
{
	if (PxSphericalJoint *p = m_joint->is<PxSphericalJoint>())
		p->setLimitCone(config);
	return true;
}


// Spherical Joint
// return cone limit configuration
physx::PxJointLimitCone cJoint::GetConeLimit()
{
	if (PxSphericalJoint *p = m_joint->is<PxSphericalJoint>())
		return p->getLimitCone();
	return physx::PxJointLimitCone(0,0,0.01f);
}


// Set SphericalJoint Flag
bool cJoint::SetSphericalJointFlag(physx::PxSphericalJointFlag::Enum flag, bool value)
{
	if (PxSphericalJoint *p = m_joint->is<PxSphericalJoint>())
		p->setSphericalJointFlag(flag, value);
	return true;
}


// set revolute joint limit configuration
bool cJoint::SetAngularLimit(const physx::PxJointAngularLimitPair &config)
{
	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
		p->setLimit(config);
	return true;
}


physx::PxJointAngularLimitPair cJoint::GetAngularLimit()
{
	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
		return p->getLimit();
	return physx::PxJointAngularLimitPair(0, 0, 0);
}


bool cJoint::IsCycleDrive() 
{
	return m_isCycleDrive;
}

bool cJoint::EnableCycleDrive(const bool enable) 
{
	m_isCycleDrive = enable;
	if (enable)
		if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
			p->setDriveVelocity(m_maxDriveVelocity);
	return true;
}

bool cJoint::SetCycleDrivePeriod(const float period, const float accel)
{
	m_cyclePeriod = period;
	m_cycleDriveAccel = accel;
	return true;
}


// return drive period, acceleration
Vector2 cJoint::GetCycleDrivePeriod()
{
	return Vector2(m_cyclePeriod, m_cycleDriveAccel);
}


// set revolute joint flag
bool cJoint::SetRevoluteJointFlag(physx::PxRevoluteJointFlag::Enum flag, bool value)
{
	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
		p->setRevoluteJointFlag(flag, value);
	return true;
}


// set revolute joint drive velocity
bool cJoint::SetDriveVelocity(const float velocity)
{
	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
	{
		p->setDriveVelocity(velocity);
		p->setDriveForceLimit(1000.f);
	}

	m_maxDriveVelocity = velocity;
	return true;
}


// return drive velocity
float cJoint::GetDriveVelocity()
{
	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
		return p->getDriveVelocity();
	return 0.f;
}


// set drive configuration
bool cJoint::EnableDrive(const bool enable)
{
	SetRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, enable);
	return true;
}


// is drive?
bool cJoint::IsDrive()
{
	RETV(!m_joint, false);

	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
	{
		const PxRevoluteJointFlags flags = p->getRevoluteJointFlags();
		const bool isDrive = flags.isSet(PxRevoluteJointFlag::eDRIVE_ENABLED);
		return isDrive;
	}
	return false;
}


// set angular limit configuration
bool cJoint::EnableAngularLimit(const bool enable)
{
	SetRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, enable);
	return true;
}


// is angular limit?
bool cJoint::IsAngularLimit()
{
	RETV(!m_joint, false);

	if (PxRevoluteJoint *p = m_joint->is<PxRevoluteJoint>())
	{
		const PxRevoluteJointFlags flags = p->getRevoluteJointFlags();
		const bool isLimit = flags.isSet(PxRevoluteJointFlag::eLIMIT_ENABLED);
		return isLimit;
	}
	return false;
}


// Prismatic Joint
// enable linear limit
bool cJoint::EnableLinearLimit(const bool enable)
{
	RETV(!m_joint, false);

	if (PxPrismaticJoint *p = m_joint->is<PxPrismaticJoint>())
		p->setPrismaticJointFlag(PxPrismaticJointFlag::eLIMIT_ENABLED, enable);
	return true;
}


// Prismatic Joint
// is enable linear limit?
bool cJoint::IsLinearLimit()
{
	RETV(!m_joint, false);

	if (PxPrismaticJoint *p = m_joint->is<PxPrismaticJoint>())
	{
		const PxPrismaticJointFlags flags = p->getPrismaticJointFlags();
		const bool isLimit = flags.isSet(PxPrismaticJointFlag::eLIMIT_ENABLED);
		return isLimit;
	}
	return false;
}


// Prismatic Joint
// set linear limit configuration
bool cJoint::SetLinearLimit(const physx::PxJointLinearLimitPair &config)
{
	RETV(!m_joint, false);

	if (PxPrismaticJoint *p = m_joint->is<PxPrismaticJoint>())
		p->setLimit(config);
	return true;
}


// Prismatic Joint
// retur linear limit configuration
physx::PxJointLinearLimitPair cJoint::GetLinearLimit()
{
	RETV(!m_joint, physx::PxJointLinearLimitPair(PxTolerancesScale(), 0, 0, 0.01f));

	if (PxPrismaticJoint *p = m_joint->is<PxPrismaticJoint>())
		return p->getLimit();
	return physx::PxJointLinearLimitPair(PxTolerancesScale(), 0, 0, 0.01f);
}


// Distance Joint
// enable distance joint limit
bool cJoint::EnableDistanceLimit(const bool enable)
{
	RETV(!m_joint, false);

	if (PxDistanceJoint *p = m_joint->is<PxDistanceJoint>())
	{
		p->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, enable);
		p->setDistanceJointFlag(PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, enable);
	}
	return true;
}


// Distance Joint
// is distance limitation?
bool cJoint::IsDistanceLimit()
{
	RETV(!m_joint, false);

	if (PxDistanceJoint *p = m_joint->is<PxDistanceJoint>())
	{
		const PxDistanceJointFlags flags = p->getDistanceJointFlags();
		const bool isLimit = flags.isSet(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED);
		return isLimit;
	}
	return true;
}


// Distance Joint
// set distance limit configuration
bool cJoint::SetDistanceLimit(const float minDist, const float maxDist)
{
	RETV(!m_joint, false);

	if (PxDistanceJoint *p = m_joint->is<PxDistanceJoint>())
	{
		p->setMinDistance(minDist);
		p->setMaxDistance(maxDist);
	}
	return true;
}


// Distance Joint
// return distance limit
// return {min distance, max distance}
Vector2 cJoint::GetDistanceLimit()
{
	RETV(!m_joint, Vector2(0,0));

	if (PxDistanceJoint *p = m_joint->is<PxDistanceJoint>())
	{
		const Vector2 dist(p->getMinDistance(), p->getMaxDistance());
		return dist;
	}
	return Vector2(0,0);
}


// modify pivot position
bool cJoint::ModifyPivot(cPhysicsEngine &physics
	, const Transform &worldTfm0, const Vector3 &pivot0
	, const Transform &worldTfm1, const Vector3 &pivot1
	, const Vector3 &revoluteAxis)
{
	if (m_actor0)
		m_actor0->RemoveJoint(this);
	if (m_actor1)
		m_actor1->RemoveJoint(this);
	PHY_SAFE_RELEASE(m_joint);

	CreateRevolute(physics, m_actor0, worldTfm0, pivot0
		, m_actor1, worldTfm1, pivot1
		, revoluteAxis);

	return true;
}


// setting default joint configuration
// breakage, projection, etc..
void cJoint::DefaultJointConfiguration(physx::PxJoint *joint)
{

	//joint->setProjectionLinearTolerance(0.1f);
	//joint->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);

	if (m_breakForce > 0.f)
		joint->setBreakForce(m_breakForce, m_breakForce);

}


// calc localFrame for PxJoint~ Function seriese
// worldTm0 : actor0 world transform
// worldTm1 : actor1 world transform
// revoluteAxis : revolution Axis
//			      if ZeroVector, ignore revolute axis
// out0 : return actor0 localFrame
// out1 : return actor1 localFrame
void cJoint::GetLocalFrame(const Transform &worldTm0, const Transform &worldTm1
	, const Vector3 &jointPos, const Vector3 &revoluteAxis
	, OUT physx::PxTransform &out0, OUT physx::PxTransform &out1)
{
	Transform tfm0 = worldTm0;
	Transform tfm1 = worldTm1;

	Vector3 p0 = jointPos - tfm0.pos;
	Vector3 p1 = jointPos - tfm1.pos;
	Quaternion q0 = tfm0.rot.Inverse();
	Quaternion q1 = tfm1.rot.Inverse();

	if (revoluteAxis != Vector3::Zeroes)
	{
		Quaternion rot(revoluteAxis, Vector3(1, 0, 0));
		tfm0.rot *= rot;
		tfm1.rot *= rot;

		p0 = (tfm0.pos - jointPos) * rot + jointPos;
		p1 = (tfm1.pos - jointPos) * rot + jointPos;
		p0 = jointPos - p0;
		p1 = jointPos - p1;
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
	if (!m_referenceMode)
	{
		PHY_SAFE_RELEASE(m_joint);
		m_actor0 = nullptr;
		m_actor1 = nullptr;
	}
}
