
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
	, const Vector3& norm
	, const Str32 &name //= "box"
)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	using namespace graphic;
	graphic::cGrid *grid = new graphic::cGrid();
	grid->Create(renderer, 1000, 1000, 1.f, 1.f
		, (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE0));
	grid->m_mtrl.InitGray4();

	cRigidActor *actor = new cRigidActor();
	actor->CreatePlane(*m_physics, norm, grid);
	m_physics->m_scene->addActor(*actor->m_actor);

	sSyncInfo *sync = new sSyncInfo;
	ZeroMemory(sync, sizeof(sSyncInfo));
	sync->id = common::GenerateId();
	sync->isRemove = true;
	sync->name = name;
	sync->actor = actor;
	sync->node = grid;
	m_syncs.push_back(sync);
	return sync->id;
}


int cPhysicsSync::SpawnBox(graphic::cRenderer &renderer
	, const Transform& tfm
	, const float density //= 1.f
	, const bool isKinematic //= false
	, const Str32 &name //= "box"
)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cCube *cube = new graphic::cCube();
	cube->Create(renderer);
	cube->SetCube(tfm);

	cRigidActor *actor = new cRigidActor();
	actor->CreateBox(*m_physics, tfm, nullptr, density, isKinematic, cube);
	m_physics->m_scene->addActor(*actor->m_actor);

	sSyncInfo *sync = new sSyncInfo;
	ZeroMemory(sync, sizeof(sSyncInfo));
	sync->id = common::GenerateId();
	sync->isRemove = true;
	sync->name = name;
	sync->actor = actor;
	sync->node = cube;
	m_syncs.push_back(sync);
	return sync->id;
}


int cPhysicsSync::SpawnSphere(graphic::cRenderer &renderer
	, const Transform &tfm
	, const float radius
	, const float density //= 1.f
	, const bool isKinematic //= false
	, const Str32 &name //= "sphere"
)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cSphere *sphere = new graphic::cSphere();
	sphere->Create(renderer, radius, 10, 10);
	sphere->m_transform.pos = tfm.pos;
	sphere->m_transform.rot = tfm.rot;

	cRigidActor *actor = new cRigidActor();
	actor->CreateSphere(*m_physics, tfm, radius, nullptr, density, isKinematic, sphere);
	m_physics->m_scene->addActor(*actor->m_actor);

	sSyncInfo *sync = new sSyncInfo;
	ZeroMemory(sync, sizeof(sSyncInfo));
	sync->id = common::GenerateId();
	sync->isRemove = true;
	sync->name = name;
	sync->actor = actor;
	sync->node = sphere;
	m_syncs.push_back(sync);
	return sync->id;
}


int cPhysicsSync::SpawnCapsule(graphic::cRenderer &renderer
	, const Transform& tfm
	, const float radius
	, const float halfHeight
	, const float density //= 1.f
	, const bool isKinematic //= false
	, const Str32 &name //= "capsule"
)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cCapsule *capsule = new graphic::cCapsule();
	capsule->Create(renderer, radius, halfHeight, 16, 8);
	capsule->m_transform.pos = tfm.pos;
	capsule->m_transform.rot = tfm.rot;

	cRigidActor *actor = new cRigidActor();
	actor->CreateCapsule(*m_physics, tfm, radius, halfHeight, nullptr, density, isKinematic, capsule);
	m_physics->m_scene->addActor(*actor->m_actor);

	sSyncInfo *sync = new sSyncInfo;
	ZeroMemory(sync, sizeof(sSyncInfo));
	sync->id = common::GenerateId();
	sync->isRemove = true;
	sync->name = name;
	sync->actor = actor;
	sync->node = capsule;
	m_syncs.push_back(sync);
	return sync->id;
}


int cPhysicsSync::SpawnCylinder(graphic::cRenderer &renderer
	, const Transform& tfm
	, const float radius
	, const float height
	, const float density //= 1.f
	, const bool isKinematic //= false
	, const Str32 &name //= "cylinder"
)
{
	RETV(!m_physics, false);
	RETV(!m_physics->m_scene, false);

	graphic::cCylinder *cylinder = new graphic::cCylinder();
	cylinder->Create(renderer, radius, height, 8);
	cylinder->m_transform.pos = tfm.pos;
	cylinder->m_transform.rot = tfm.rot;

	cRigidActor *actor = new cRigidActor();
	actor->CreateCylinder(*m_physics, tfm, radius, height, nullptr, density, isKinematic, cylinder);
	m_physics->m_scene->addActor(*actor->m_actor);

	sSyncInfo *sync = new sSyncInfo;
	ZeroMemory(sync, sizeof(sSyncInfo));
	sync->id = common::GenerateId();
	sync->isRemove = true;
	sync->name = name;
	sync->actor = actor;
	sync->node = cylinder;
	m_syncs.push_back(sync);
	return sync->id;
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

		auto it = find_if(m_syncs.begin(), m_syncs.end(), [&](const auto &a) {
			return (a->actor && (a->actor->m_actor == activeTfm->actor)); });
		if (m_syncs.end() == it)
			continue;

		sSyncInfo *sync = *it;
		{
			Transform tfm = sync->node->m_transform;
			tfm.pos = *(Vector3*)&activeTfm->actor2World.p;
			tfm.rot = *(Quaternion*)&activeTfm->actor2World.q;
			sync->node->m_transform = tfm;
		}
	}

	return true;
}


void cPhysicsSync::onRelease(const physx::PxBase* observed, void* userData
	, physx::PxDeletionEventFlag::Enum deletionEvent)
{
	// nothing~
}


bool cPhysicsSync::AddJoint(cJoint *joint
	, graphic::cNode *node // = nullptr
	, const bool isAutoRemove // = true
)
{
	if (FindSyncInfo(joint))
		return false; // already exist

	sSyncInfo *sync = new sSyncInfo;
	ZeroMemory(sync, sizeof(sSyncInfo));
	sync->id = common::GenerateId();
	sync->isRemove = isAutoRemove;
	sync->name = "joint";
	sync->joint = joint;
	sync->node = node;
	m_syncs.push_back(sync);
	return true;
}


// find actor info from id
sSyncInfo* cPhysicsSync::FindSyncInfo(const int syncId)
{
	auto it = find_if(m_syncs.begin(), m_syncs.end(), [&](const auto &a) {
		return a->id == syncId; });
	if (m_syncs.end() == it)
		return nullptr;
	return *it;
}


// find actor info from actor ptr
sSyncInfo* cPhysicsSync::FindSyncInfo(const cRigidActor *actor)
{
	auto it = find_if(m_syncs.begin(), m_syncs.end(), [&](const auto &a) {
		return a->actor == actor; });
	if (m_syncs.end() == it)
		return nullptr;
	return *it;
}


// find actor info from joint ptr
sSyncInfo* cPhysicsSync::FindSyncInfo(const cJoint *joint)
{
	auto it = find_if(m_syncs.begin(), m_syncs.end(), [&](const auto &a) {
		return a->joint == joint; });
	if (m_syncs.end() == it)
		return nullptr;
	return *it;
}


// remove joint and connections
bool cPhysicsSync::RemoveSyncInfo(const int syncId)
{
	sSyncInfo *sync = FindSyncInfo(syncId);
	if (!sync)
		return false; // not exist
	return RemoveSyncInfo(sync);
}


// remove joint and connections
bool cPhysicsSync::RemoveSyncInfo(sSyncInfo *sync)
{
	common::removevector(m_syncs, sync);

	if (sync->joint) // joint type?
	{
		// remove joint reference
		// todo: check sync->joint->m_actor0 available
		if (sync->joint->m_actor0)
			sync->joint->m_actor0->RemoveJoint(sync->joint);
		if (sync->joint->m_actor1)
			sync->joint->m_actor1->RemoveJoint(sync->joint);
	}

	if (sync->isRemove)
	{
		SAFE_DELETE(sync->actor);
		SAFE_DELETE(sync->joint);
		SAFE_DELETE(sync->node);
	}
	SAFE_DELETE(sync);
	return true;
}


// remove joint and connections
bool cPhysicsSync::RemoveSyncInfo(const cRigidActor *actor)
{
	sSyncInfo *sync = FindSyncInfo(actor);
	if (!sync)
		return false; // not exist
	return RemoveSyncInfo(sync);
}


// remove joint and connections
bool cPhysicsSync::RemoveSyncInfo(const cJoint *joint)
{
	sSyncInfo *sync = FindSyncInfo(joint);
	if (!sync)
		return false; // not exist
	return RemoveSyncInfo(sync);
}


// clear physics actor, joint object
void cPhysicsSync::Clear()
{
	if (m_physics)
		m_physics->m_physics->unregisterDeletionListener(*this);

	// remove actor
	for (auto &p : m_syncs)
	{
		if (m_physics && m_physics->m_scene && p->actor)
			m_physics->m_scene->removeActor(*p->actor->m_actor);
		if (p->isRemove)
		{
			SAFE_DELETE(p->actor);
			SAFE_DELETE(p->joint);
			SAFE_DELETE(p->node);
		}
		delete p;
	}
	m_syncs.clear();

	_aligned_free(m_bufferedActiveTransforms);
}
