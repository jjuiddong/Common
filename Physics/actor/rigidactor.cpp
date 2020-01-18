
#include "stdafx.h"
#include "rigidactor.h"
#include "../manager/physicsengine.h"

using namespace phys;
using namespace physx;


cRigidActor::cRigidActor()
	: m_actor(nullptr)
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
	physics.m_scene->addActor(*plane);

	m_type = eType::Static;
	m_shape = eShape::Plane;
	m_actor = plane;
	return true;
}


bool cRigidActor::CreateBox(cPhysicsEngine &physics
	, const Vector3& pos
	, const Vector3& dims
	, const Vector3* linVel //= nullptr
	, float density //= 1.f
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidDynamic* box = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&pos)
		, PxBoxGeometry(*(PxVec3*)&dims), *physics.m_material, density);
	PX_ASSERT(box);

	box->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	box->setAngularDamping(0.5f);
	box->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	physics.m_scene->addActor(*box);
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
	, float density //= 1.f
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	PxRigidDynamic* sphere = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&pos)
		, PxSphereGeometry(radius), *physics.m_material, density);
	PX_ASSERT(sphere);

	sphere->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	sphere->setAngularDamping(0.5f);
	sphere->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	physics.m_scene->addActor(*sphere);

	if (linVel)
		sphere->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eType::Dynamic;
	m_shape = eShape::Sphere;
	m_actor = sphere;
	return true;
}


bool cRigidActor::CreateCapsule(cPhysicsEngine &physics
	, const Vector3& pos
	, const float radius
	, const float halfHeight
	, const Vector3* linVel //= nullptr
	, float density //= 1.f
)
{
	PxSceneWriteLock scopedLock(*physics.m_scene);
	const PxQuat rot = PxQuat(PxIdentity);
	PX_UNUSED(rot);

	PxRigidDynamic* capsule = PxCreateDynamic(*physics.m_physics
		, PxTransform(*(PxVec3*)&pos)
		, PxCapsuleGeometry(radius, halfHeight), *physics.m_material, density);
	PX_ASSERT(capsule);

	capsule->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	capsule->setAngularDamping(0.5f);
	capsule->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	physics.m_scene->addActor(*capsule);

	if (linVel)
		capsule->setLinearVelocity(*(PxVec3*)linVel);

	m_type = eType::Dynamic;
	m_shape = eShape::Capsule;
	m_actor = capsule;
	return true;
}


void cRigidActor::Clear()
{
	PHY_SAFE_RELEASE(m_actor);
}
