
#include "stdafx.h"
#include "rigidactor.h"
#include "../manager/physicsengine.h"

using namespace phys;
using namespace physx;


cRigidActor::cRigidActor()
	: m_id(common::GenerateId())
	, m_actor(nullptr)
	, m_type(eType::None)
	, m_shape(eShape::None)
{
}

cRigidActor::~cRigidActor()
{
	Clear();
}


bool cRigidActor::CreatePlane(cPhysicsEngine &physics
	, const Vector3 &norm)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidStatic* plane = PxCreatePlane(*physics.m_physics
		, PxPlane(*(PxVec3*)&norm, 0), *physics.m_material);
	if (!plane)
		return nullptr;

	m_type = eType::Static;
	m_shape = eShape::Plane;
	m_actor = plane;
	return true;
}


bool cRigidActor::CreateBox(cPhysicsEngine &physics
	, const Transform &tfm
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidDynamic* box = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot)
		, PxBoxGeometry(*(PxVec3*)&tfm.scale), *physics.m_material, density);
	PX_ASSERT(box);

	box->setLinearDamping(1.f);
	box->setAngularDamping(1.f);
	//box->setAngularDamping(3.f);
	//box->setAngularDamping(0.5f);
	box->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	box->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	if (linVel)
		box->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eType::Dynamic;
	m_shape = eShape::Box;
	m_actor = box;
	return true;
}


bool cRigidActor::CreateSphere(cPhysicsEngine &physics
	, const Vector3& pos
	, const float radius
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidDynamic* sphere = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&pos)
		, PxSphereGeometry(radius), *physics.m_material, density);
	PX_ASSERT(sphere);

	sphere->setLinearDamping(1.f);
	sphere->setAngularDamping(1.f);
	sphere->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	sphere->setActorFlag(PxActorFlag::eVISUALIZATION, true);

	if (linVel)
		sphere->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eType::Dynamic;
	m_shape = eShape::Sphere;
	m_actor = sphere;
	return true;
}


bool cRigidActor::CreateCapsule(cPhysicsEngine &physics
	, const Transform &tfm
	, const float radius
	, const float halfHeight
	, const Vector3* linVel //= nullptr
	, const float density //= 1.f
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	const PxQuat rot = PxQuat(PxIdentity);
	PX_UNUSED(rot);

	PxRigidDynamic* capsule = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&tfm.pos, *(PxQuat*)&tfm.rot)
		, PxCapsuleGeometry(radius, halfHeight), *physics.m_material, density);
	PX_ASSERT(capsule);

	capsule->setLinearDamping(1.f);
	capsule->setAngularDamping(1.f);
	capsule->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	capsule->setActorFlag(PxActorFlag::eVISUALIZATION, true);

	if (linVel)
		capsule->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eType::Dynamic;
	m_shape = eShape::Capsule;
	m_actor = capsule;
	return true;
}


// change geometry shape
bool cRigidActor::ChangeDimension(cPhysicsEngine &physics, const Vector3 &dim)
{
	RETV(!m_actor, false);

	// 1. remove old shape
	const uint count = m_actor->getNbShapes();
	if (count == 0)
		return false;

	PxShape **shapes = new PxShape*[count];
	const uint result = m_actor->getShapes(shapes, count);
	for (uint i = 0; i < result; ++i)
		m_actor->detachShape(*shapes[i]);
	delete[] shapes;

	// 2. generate new shape
	switch (m_shape)
	{
	case eShape::Box:
		m_actor->createShape(PxBoxGeometry(*(PxVec3*)&dim), *physics.m_material); 
		break;
	case eShape::Sphere:
		m_actor->createShape(PxSphereGeometry(dim.x), *physics.m_material);
		break;
	case eShape::Capsule:
		m_actor->createShape(PxCapsuleGeometry(dim.y, dim.x), *physics.m_material);
		break;

	case eShape::Plane: // not support
	case eShape::Convex: // not support
		assert(0);
		break;
	default: assert(0); break;
	}

	return true;
}


bool cRigidActor::SetGlobalPose(const physx::PxTransform &tfm)
{
	PxRigidDynamic *p = m_actor->is<PxRigidDynamic>();
	RETV(!p, false);

	const PxRigidBodyFlags flags = p->getRigidBodyFlags();
	const bool isKinematic = flags.isSet(PxRigidBodyFlag::eKINEMATIC);
	if (isKinematic)
		p->setKinematicTarget(tfm);
	else
		p->setGlobalPose(tfm);
	return true;
}


bool cRigidActor::SetGlobalPose(const Transform &tfm)
{
	const PxTransform target = PxTransform(*(PxQuat*)&tfm.rot) * PxTransform(*(PxVec3*)&tfm.pos);
	return SetGlobalPose(target);
}


bool cRigidActor::SetKinematic(const bool isKinematic)
{
	RETV(!m_actor, false);
	RETV(m_type != eType::Dynamic, false);

	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);

	return true;
}


bool cRigidActor::IsKinematic() const
{
	RETV(!m_actor, false);
	RETV(m_type != eType::Dynamic, false);

	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
	{
		const PxRigidBodyFlags flags = p->getRigidBodyFlags();
		const bool isKinematic = flags.isSet(PxRigidBodyFlag::eKINEMATIC);
		return isKinematic;
	}
	return true;
}


float cRigidActor::GetMass() const
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		return p->getMass();
	return 0.f;
}


bool cRigidActor::SetMass(const float mass)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setMass(mass);
	return true;
}


float cRigidActor::GetLinearDamping() const
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		return p->getLinearDamping();
	return 0.f;
}


bool cRigidActor::SetLinearDamping(const float damping)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setLinearDamping(damping);
	return true;
}


float cRigidActor::GetAngularDamping() const
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		return p->getAngularDamping();
	return 0.f;
}


bool cRigidActor::SetAngularDamping(const float damping)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setAngularDamping(damping);
	return true;
}


bool cRigidActor::SetMaxAngularVelocity(const float maxVel)
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->setMaxAngularVelocity(maxVel);
	return true;
}


float cRigidActor::GetMaxAngularVelocity()
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		return p->getMaxAngularVelocity();
	return 0.f;
}


bool cRigidActor::WakeUp()
{
	if (PxRigidDynamic *p = m_actor->is<PxRigidDynamic>())
		p->wakeUp();
	return true;
}


// add joint reference
// joint is this rigid actor connection joint
bool cRigidActor::AddJoint(cJoint *joint)
{
	if (m_joints.end() == std::find(m_joints.begin(), m_joints.end(), joint))
	{
		m_joints.push_back(joint);
		return true;
	}
	return false;
}


// remove joint reference
bool cRigidActor::RemoveJoint(cJoint *joint)
{
	if (m_joints.end() != std::find(m_joints.begin(), m_joints.end(), joint))
	{
		common::removevector(m_joints, joint);
		return true;
	}
	return false;
}


void cRigidActor::Clear()
{
	m_joints.clear();
	PHY_SAFE_RELEASE(m_actor);
}
