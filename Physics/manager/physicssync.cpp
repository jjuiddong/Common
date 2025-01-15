
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


// if ground plane, name is "ground"
int cPhysicsSync::SpawnPlane(graphic::cRenderer &renderer
	, const Vector3& norm
	, const Str32 &name //= "plane"
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
	actor->m_actor->userData = sync; // update user data
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
	actor->m_actor->userData = sync; // update user data
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
	actor->m_actor->userData = sync; // update user data
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
	actor->m_actor->userData = sync; // update user data
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
	actor->m_actor->userData = sync; // update user data
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

	// https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyDynamics.html
	uint size = 0;
	{
		//PxSceneReadLock scopedLock(*scene);

		PxActor** activeActors = scene->getActiveActors(size);

		for (uint i = 0; i < size; ++i)
		{
			sSyncInfo* sync = (sSyncInfo*)activeActors[i]->userData;
			if (sync && sync->actor && sync->actor->m_actor)
			{
				const PxTransform gtm = sync->actor->m_actor->getGlobalPose();

				Transform tfm;
				tfm.pos = *(Vector3*)&gtm.p;
				tfm.rot = *(Quaternion*)&gtm.q;

				cRigidActor* actor = sync->actor;
				for (uint i = 0; i < actor->m_meshes.size(); ++i)
				{
					graphic::cNode* node = actor->m_meshes[i].node;
					tfm.scale = node->m_transform.scale;
					node->m_transform = tfm;
				}
			}
		}
	}


	// update active actor buffer
	//uint activeActorSize = 0;
	//{
	//	PxSceneReadLock scopedLock(*scene);
	//	const PxActiveTransform* activeTransforms =
	//		scene->getActiveTransforms(activeActorSize);
	//	if (activeActorSize > m_activeBufferCapacity)
	//	{
	//		_aligned_free(m_bufferedActiveTransforms);

	//		m_activeBufferCapacity = activeActorSize;
	//		m_bufferedActiveTransforms = (physx::PxActiveTransform*)_aligned_malloc(
	//			sizeof(physx::PxActiveTransform) * activeActorSize, 16);
	//	}

	//	if (activeActorSize > 0)
	//	{
	//		PxMemCopy(m_bufferedActiveTransforms, activeTransforms
	//			, sizeof(PxActiveTransform) * activeActorSize);
	//	}
	//}

	//// update render object
	//for (uint i = 0; i < activeActorSize; ++i)
	//{
	//	PxActiveTransform *activeTfm = &m_bufferedActiveTransforms[i];

	//	auto it = find_if(m_syncs.begin(), m_syncs.end(), [&](const auto &a) {
	//		return (a->actor && (a->actor->m_actor == activeTfm->actor)); });
	//	if (m_syncs.end() == it)
	//		continue;

	//	sSyncInfo *sync = *it;
	//	{
	//		Transform tfm;
	//		tfm.pos = *(Vector3*)&activeTfm->actor2World.p;
	//		tfm.rot = *(Quaternion*)&activeTfm->actor2World.q;
	//		const Matrix44 tm0 = tfm.GetMatrix();			

	//		if (sync->actor)
	//		{
	//			cRigidActor *actor = sync->actor;
	//			for (uint i=0; i < actor->m_meshes.size(); ++i)
	//			{
	//				graphic::cNode *node = actor->m_meshes[i].node;
	//				const Matrix44 tm = actor->m_meshes[i].local * tm0;

	//				Transform m;
	//				m.pos = tm.GetPosition();
	//				m.rot = tm.GetQuaternion();
	//				m.scale = node->m_transform.scale;
	//				node->m_transform = m;
	//			}
	//		}
	//	}
	//}

	return true;
}


void cPhysicsSync::onRelease(const physx::PxBase* observed, void* userData
	, physx::PxDeletionEventFlag::Enum deletionEvent)
{
	// nothing~
}


bool cPhysicsSync::AddJoint(cJoint *j1
	, graphic::cNode *node // = nullptr
	, const bool isAutoRemove // = true
)
{
	if (FindSyncInfo(j1))
		return false; // already exist

	sSyncInfo *sync = new sSyncInfo;
	ZeroMemory(sync, sizeof(sSyncInfo));
	sync->id = common::GenerateId();
	sync->isRemove = isAutoRemove;
	sync->name = "joint";
	sync->j1 = j1;
	sync->node = node;
	j1->m_node = node;
	m_syncs.push_back(sync);
	return true;
}


// compoound two sync info, add sync1 to sync0
// and then remove sync1
bool cPhysicsSync::AddCompound(sSyncInfo *sync0, sSyncInfo *sync1)
{
	if (!sync0->actor || !sync1->actor)
		return false;

	sync0->actor->Compound(*m_physics, sync1->actor);
	sync1->node = nullptr;
	RemoveSyncInfo(sync1);

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
sSyncInfo* cPhysicsSync::FindSyncInfo(const cJoint *j1)
{
	auto it = find_if(m_syncs.begin(), m_syncs.end(), [&](const auto &a) {
		return a->j1 == j1; });
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

	if (sync->j1) // joint type?
	{
		// remove joint reference
		// todo: check sync->joint->m_actor0 available
		if (sync->j1->m_actor0)
			sync->j1->m_actor0->RemoveJoint(sync->j1);
		if (sync->j1->m_actor1)
			sync->j1->m_actor1->RemoveJoint(sync->j1);
	}

	if (sync->isRemove)
	{
		SAFE_DELETE(sync->actor);
		SAFE_DELETE(sync->j1);
		//SAFE_DELETE(sync->node);
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
bool cPhysicsSync::RemoveSyncInfo(const cJoint *j1)
{
	sSyncInfo *sync = FindSyncInfo(j1);
	if (!sync)
		return false; // not exist
	return RemoveSyncInfo(sync);
}


// clear sync object
// isClearGroundPlane: true = all sync info removed
//		               false = all sync info removed except ground plane
void cPhysicsSync::ClearSyncInfo(
	const bool isClearGroundPlane //= true
)
{
	for (int i = (int)m_syncs.size()-1; i >= 0; --i)
	{
		sSyncInfo *p = m_syncs[i];

		if (!isClearGroundPlane && (p->name == "ground"))
			continue; // no remove ground plane

		if (m_physics && m_physics->m_scene && p->actor)
			m_physics->m_scene->removeActor(*p->actor->m_actor);
		if (p->isRemove)
		{
			SAFE_DELETE(p->actor);
			SAFE_DELETE(p->j1);
		}
		delete p;

		common::removevector(m_syncs, p);
	}
}


// clear physics actor, joint object
void cPhysicsSync::Clear()
{
	if (m_physics)
		m_physics->m_physics->unregisterDeletionListener(*this);

	ClearSyncInfo();

	_aligned_free(m_bufferedActiveTransforms);
}
