
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


int cPhysicsSync::SpawnPlane(graphic::cRenderer &renderer
	, const Vector3& norm)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	using namespace graphic;
	graphic::cGrid *grid = new graphic::cGrid();
	grid->Create(renderer, 200, 200, 1.f, 1.f
		, (eVertexType::POSITION | eVertexType::NORMAL));
	grid->m_mtrl.InitGray4();

	cRigidActor *actor = new cRigidActor();
	actor->CreatePlane(*m_physics, norm);
	m_physics->m_scene->addActor(*actor->m_actor);

	sActorInfo *info = new sActorInfo;
	info->id = common::GenerateId();
	info->name = "plane";
	info->actor = actor;
	info->node = grid;
	m_actors.push_back(info);
	return info->id;
}


int cPhysicsSync::SpawnBox(graphic::cRenderer &renderer
	, const Transform& tfm
	, const float density //= 1.f
)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cCube *cube = new graphic::cCube();
	cube->Create(renderer);
	cube->SetCube(tfm);

	cRigidActor *actor = new cRigidActor();
	actor->CreateBox(*m_physics, tfm, nullptr, density);
	m_physics->m_scene->addActor(*actor->m_actor);

	sActorInfo *info = new sActorInfo;
	info->id = common::GenerateId();
	info->name = "box";
	info->actor = actor;
	info->node = cube;
	m_actors.push_back(info);
	return info->id;
}


int cPhysicsSync::SpawnSphere(graphic::cRenderer &renderer
	, const Vector3& pos
	, const float radius
	, const float density //= 1.f
)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cSphere *sphere = new graphic::cSphere();
	sphere->Create(renderer, radius, 10, 10);
	sphere->m_transform.pos = pos;

	cRigidActor *actor = new cRigidActor();
	actor->CreateSphere(*m_physics, pos, radius, nullptr, density);
	m_physics->m_scene->addActor(*actor->m_actor);

	sActorInfo *info = new sActorInfo;
	info->id = common::GenerateId();
	info->name = "sphere";
	info->actor = actor;
	info->node = sphere;
	m_actors.push_back(info);
	return info->id;
}


int cPhysicsSync::SpawnCapsule(graphic::cRenderer &renderer
	, const Transform& tfm
	, const float radius
	, const float halfHeight
	, const float density //= 1.f
)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cCapsule *capsule = new graphic::cCapsule();
	capsule->Create(renderer, radius, halfHeight, 16, 8);
	capsule->m_transform.pos = tfm.pos;

	cRigidActor *actor = new cRigidActor();
	actor->CreateCapsule(*m_physics, tfm, radius, halfHeight, nullptr, density);
	m_physics->m_scene->addActor(*actor->m_actor);

	sActorInfo *info = new sActorInfo;
	info->id = common::GenerateId();
	info->name = "capsule";
	info->actor = actor;
	info->node = capsule;
	m_actors.push_back(info);
	return info->id;
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
			return (a->actor->m_actor == activeTfm->actor); });
		if (m_actors.end() == it)
			continue;

		sActorInfo *actor = *it;
		{
			Transform tfm = actor->node->m_transform;
			tfm.pos = *(Vector3*)&activeTfm->actor2World.p;
			tfm.rot = *(Quaternion*)&activeTfm->actor2World.q;
			actor->node->m_transform = tfm;
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


// remove joint and connections
bool cPhysicsSync::RemoveJoint(cJoint *joint)
{
	if (joint->m_actor0)
		joint->m_actor0->RemoveJoint(joint);
	if (joint->m_actor1)
		joint->m_actor1->RemoveJoint(joint);
	common::removevector(m_joints, joint);
	delete joint;
	return true;
}


// find actor info from id
sActorInfo* cPhysicsSync::FindActorInfo(const int id)
{
	auto it = find_if(m_actors.begin(), m_actors.end(), [&](const auto &a) {
		return a->id == id; });
	if (m_actors.end() == it)
		return nullptr;
	return *it;
}


// find actor info from actor ptr
sActorInfo* cPhysicsSync::FindActorInfo(const cRigidActor *actor)
{
	auto it = find_if(m_actors.begin(), m_actors.end(), [&](const auto &a) {
		return a->actor == actor; });
	if (m_actors.end() == it)
		return nullptr;
	return *it;
}


// clear physics actor, joint object
void cPhysicsSync::Clear()
{
	if (m_physics)
		m_physics->m_physics->unregisterDeletionListener(*this);

	for (auto &j : m_joints)
		SAFE_DELETE(j);
	m_joints.clear();

	for (auto &p : m_actors)
	{
		if (m_physics && m_physics->m_scene)
			m_physics->m_scene->removeActor(*p->actor->m_actor);
		SAFE_DELETE(p->actor);
		SAFE_DELETE(p->node);
		delete p;
	}
	m_actors.clear();

	_aligned_free(m_bufferedActiveTransforms);
}
