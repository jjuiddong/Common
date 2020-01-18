
#include "stdafx.h"
#include "physicssync.h"

using namespace phys;


cPhysicsSync::cPhysicsSync()
	: m_physics(nullptr)
{
}

cPhysicsSync::~cPhysicsSync()
{
	Clear();
}


bool cPhysicsSync::Create(cPhysicsEngine *physics)
{
	m_physics = physics;
	physics->SetPhysicsSync(this);
	physics->m_physics->registerDeletionListener(*this, physx::PxDeletionEventFlag::eUSER_RELEASE);
	return true;
}


graphic::cGridLine* cPhysicsSync::SpawnPlane(graphic::cRenderer &renderer
	, const Vector3& norm)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cGridLine *grid = new graphic::cGridLine();
	grid->Create(renderer, 100, 100, 1.f, 1.f);

	cRigidActor *actor = new cRigidActor();
	actor->CreatePlane(*m_physics, norm);
	m_physics->m_scene->addActor(*actor->m_actor);

	m_actors.push_back({ common::GenerateId(), "plane", actor, grid });
	return grid;
}


graphic::cCube* cPhysicsSync::SpawnBox(graphic::cRenderer &renderer
	, const Vector3& pos
	, const Vector3 &scale)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cCube *cube = new graphic::cCube();
	cube->Create(renderer);
	Transform tfm;
	tfm.pos = pos;
	tfm.scale = scale;
	cube->SetCube(tfm);

	cRigidActor *actor = new cRigidActor();
	actor->CreateBox(*m_physics, pos, scale);
	m_physics->m_scene->addActor(*actor->m_actor);

	m_actors.push_back({ common::GenerateId(), "box", actor, cube });
	return cube;
}


graphic::cSphere* cPhysicsSync::SpawnSphere(graphic::cRenderer &renderer
	, const Vector3& pos
	, const float radius)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cSphere *sphere = new graphic::cSphere();
	sphere->Create(renderer, radius, 10, 10);
	sphere->m_transform.pos = pos;

	cRigidActor *actor = new cRigidActor();
	actor->CreateSphere(*m_physics, pos, radius);
	m_physics->m_scene->addActor(*actor->m_actor);

	m_actors.push_back({ common::GenerateId(), "sphere", actor, sphere });
	return sphere;
}


graphic::cCapsule* cPhysicsSync::SpawnCapsule(graphic::cRenderer &renderer
	, const Vector3& pos
	, const float radius
	, const float halfHeight)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cCapsule *capsule = new graphic::cCapsule();
	capsule->Create(renderer, radius, halfHeight, 16, 8);
	capsule->SetPos(pos);

	cRigidActor *actor = new cRigidActor();
	actor->CreateCapsule(*m_physics, pos, radius, halfHeight);
	m_physics->m_scene->addActor(*actor->m_actor);

	m_actors.push_back({ common::GenerateId(), "capsule", actor, capsule });
	return capsule;
}


// physics object sync
bool cPhysicsSync::Sync()
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);
	physx::PxScene *scene = m_physics->m_scene;

	using namespace physx;

	// update active actor buffer
	uint activeActorSize = 0;
	{
		PxSceneReadLock scopedLock(*scene);
		const PxActiveTransform* activeTransforms =
			scene->getActiveTransforms(activeActorSize);
		if (activeActorSize > m_activeBufferCapacity)
		{
			_aligned_free(m_bufferedActiveTransforms);

			m_activeBufferCapacity = activeActorSize;
			m_bufferedActiveTransforms = (physx::PxActiveTransform*)_aligned_malloc(
				sizeof(physx::PxActiveTransform) * activeActorSize, 16);
		}

		if (activeActorSize > 0)
		{
			PxMemCopy(m_bufferedActiveTransforms, activeTransforms
				, sizeof(PxActiveTransform) * activeActorSize);
		}
	}

	// update render object
	for (uint i = 0; i < activeActorSize; ++i)
	{
		PxActiveTransform *activeTfm = &m_bufferedActiveTransforms[i];

		auto it = find_if(m_actors.begin(), m_actors.end(), [&](const auto &a) {
			return (a.actor->m_actor == activeTfm->actor); });
		if (m_actors.end() == it)
			continue;

		sActor &actor = *it;
		{
			Transform tfm = actor.node->m_transform;
			tfm.pos = *(Vector3*)&activeTfm->actor2World.p;
			tfm.rot = *(Quaternion*)&activeTfm->actor2World.q;
			actor.node->m_transform = tfm;
		}
	}

	return true;
}


void cPhysicsSync::onRelease(const physx::PxBase* observed, void* userData
	, physx::PxDeletionEventFlag::Enum deletionEvent)
{
	// nothing~
}


bool cPhysicsSync::AddJoint(cJoint *joint)
{
	m_joints.push_back(joint);
	return true;
}


// clear physics actor, joint object
void cPhysicsSync::Clear()
{
	if (m_physics)
		m_physics->m_physics->unregisterDeletionListener(*this);

	for (auto &p : m_actors)
	{
		if (m_physics && m_physics->m_scene)
			m_physics->m_scene->removeActor(*p.actor->m_actor);
		SAFE_DELETE(p.actor);
		SAFE_DELETE(p.node);
	}
	m_actors.clear();

	for (auto &j : m_joints)
		SAFE_DELETE(j);
	m_joints.clear();

	_aligned_free(m_bufferedActiveTransforms);
}
