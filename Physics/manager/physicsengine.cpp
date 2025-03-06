
#include "stdafx.h"
#include "physicsengine.h"


using namespace phys;
using namespace physx;


cPhysicsEngine::cPhysicsEngine()
	: m_objSync(nullptr)
	, m_scratchBlock(nullptr)
	, m_scratchBlockSize(1024 * 128)
	, m_isPVD(false)
	, m_stepSize(1.f/50.f)
	, m_accTime(0.f)
	, m_isFetch(false)
	, m_timerScale(1.f)
{
}

cPhysicsEngine::~cPhysicsEngine()
{
	Clear();
}


bool cPhysicsEngine::InitializePhysx()
{
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION//PX_FOUNDATION_VERSION
		, m_defaultAllocatorCallback, m_defaultErrorCallback);
	if (!m_foundation)
		return false;

	// pvd connection
	if (m_isPVD)
	{
		sPvdParameters pvdParams;
		m_transport = physx::PxDefaultPvdSocketTransportCreate(pvdParams.ip.c_str(), pvdParams.port
			, pvdParams.timeout);
		if (m_transport == NULL)
			return false;
		m_pvdFlags = physx::PxPvdInstrumentationFlag::eALL;
		if (!pvdParams.useFullPvdConnection)
			m_pvdFlags = physx::PxPvdInstrumentationFlag::ePROFILE;
		m_pvd = physx::PxCreatePvd(*m_foundation);
		m_pvd->connect(*m_transport, m_pvdFlags);
	}//~pvd

	bool recordMemoryAllocations = true;
	physx::PxTolerancesScale scale;
	//scale.length = 10;
	//scale.speed = 10;
	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, scale
		, recordMemoryAllocations, m_pvd);

	m_physics->registerDeletionListener(*this, physx::PxDeletionEventFlag::eUSER_RELEASE);

	m_material = m_physics->createMaterial(0.5f, 0.5f, 0.1f);
	if (!m_material)
		return false;

	physx::PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildGPUData = true; //Enable GRB data being produced in cooking.

	// scene initialize
	physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	if (!sceneDesc.cpuDispatcher)
	{
		const uint numThreads = 1;
		m_cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(numThreads);
		if (!m_cpuDispatcher)
			return false;
		sceneDesc.cpuDispatcher = m_cpuDispatcher;
	}

	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	physx::PxCudaContextManagerDesc cudaContextManagerDesc;
	m_cudaContextManager = PxCreateCudaContextManager(*m_foundation, cudaContextManagerDesc);
	if (m_cudaContextManager)
	{
		if (!m_cudaContextManager->contextIsValid())
		{
			PHY_SAFE_RELEASE(m_cudaContextManager);
		}
	}
	//if (!sceneDesc.gpuDispatcher && m_cudaContextManager)
	//	sceneDesc.gpuDispatcher = m_cudaContextManager->getGpuDispatcher();

	//sceneDesc.frictionType = physx::PxFrictionType::eTWO_DIRECTIONAL;
	//sceneDesc.frictionType = physx::PxFrictionType::eONE_DIRECTIONAL;
	//sceneDesc.flags |= physx::PxSceneFlag::eENABLE_AVERAGE_POINT;
	//sceneDesc.flags |= physx::PxSceneFlag::eADAPTIVE_FORCE;
	//sceneDesc.flags |= physx::PxSceneFlag::eDISABLE_CONTACT_CACHE;
	//sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;
	//sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVETRANSFORMS; // Physx 3.4
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS; // Physx 4.0
	sceneDesc.sceneQueryUpdateMode = physx::PxSceneQueryUpdateMode::eBUILD_ENABLED_COMMIT_DISABLED;
	sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = 8;

	m_scene = m_physics->createScene(sceneDesc);
	if (!m_scene)
		return false;

	physx::PxSceneWriteLock scopedLock(*m_scene);
	physx::PxSceneFlags flag = m_scene->getFlags();
	PX_UNUSED(flag);
	const bool IsinitialDebugRender = false;
	const float debugRenderScale = 1.f;
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE
		, IsinitialDebugRender ? debugRenderScale : 0.0f);
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	//m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

	physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	if (m_scratchBlockSize > 0)
		m_scratchBlock = _aligned_malloc(m_scratchBlockSize, 16);

	return true;
}


bool cPhysicsEngine::PreUpdate(const float deltaSeconds)
{
	const float step = 1.f / 60.f;
	m_accTime += deltaSeconds * m_timerScale;
	if (m_accTime > step)
	{
		m_accTime -= step;
		physx::PxSceneWriteLock writeLock(*m_scene);
		m_scene->simulate(step, nullptr, m_scratchBlock, m_scratchBlockSize);
		m_isFetch = true;
	}

	// not use stepper
	//m_accTime += min(deltaSeconds, m_stepSize);
	//if (m_accTime < m_stepSize)
	//	return false;
	//m_accTime -= m_stepSize;


	//const float dt = deltaSeconds * m_timerScale;
	//if (dt == 0)
	//	return true;
	//m_stepSize = max(0.01f, dt); // if too small dt, insane physics simulation
	//m_isFetch = true;
	//physx::PxSceneWriteLock writeLock(*m_scene);
	//m_scene->simulate(m_stepSize, nullptr, m_scratchBlock, m_scratchBlockSize);
	return true;
}


// update physx result info
bool cPhysicsEngine::PostUpdate(const float deltaSeconds)
{
	if (m_isFetch)
	{
		m_isFetch = false;

		PxSceneWriteLock writeLock(*m_scene);
		m_scene->fetchResults(true);
		if (m_objSync)
			m_objSync->Sync();
	}
	return true;
}


// PxDeletionListener interface
void cPhysicsEngine::onRelease(const physx::PxBase* observed, void* userData
	, physx::PxDeletionEventFlag::Enum deletionEvent)
{
	// nothing~
}


bool cPhysicsEngine::SetPhysicsSync(iPhysicsSync *sync)
{
	SAFE_DELETE(m_objSync);
	m_objSync = sync;
	return true;
}


bool cPhysicsEngine::SceneLockWrite()
{
	RETV(!m_scene, false);
	m_scene->lockWrite();
	return true;
}


bool cPhysicsEngine::SceneUnlockWrite()
{
	RETV(!m_scene, false);
	m_scene->unlockWrite();
	return true;
}


// simuation start
bool cPhysicsEngine::Play()
{
	m_timerScale = 1.f;
	return true;
}


// simulation pause
bool cPhysicsEngine::Pause()
{
	m_timerScale = 0.f;
	return true;
}


void cPhysicsEngine::Clear()
{
	SAFE_DELETE(m_objSync);
	if (m_scratchBlock)
	{
		_aligned_free(m_scratchBlock);
		m_scratchBlock = nullptr;
	}
	if (m_physics)
		m_physics->unregisterDeletionListener(*this);
	PHY_SAFE_RELEASE(m_scene);
	PHY_SAFE_RELEASE(m_cudaContextManager);
	PHY_SAFE_RELEASE(m_cpuDispatcher);
	PHY_SAFE_RELEASE(m_material);
	PHY_SAFE_RELEASE(m_physics);
	PHY_SAFE_RELEASE(m_pvd);
	PHY_SAFE_RELEASE(m_transport);
	PHY_SAFE_RELEASE(m_foundation);
}
