
#include "stdafx.h"
#include "physicsengine.h"


using namespace phys;
using namespace physx;


cPhysicsEngine::cPhysicsEngine()
	: m_objSync(nullptr)
{
}

cPhysicsEngine::~cPhysicsEngine()
{
	Clear();
}


bool cPhysicsEngine::InitializePhysx()
{
	m_foundation = PxCreateFoundation(PX_FOUNDATION_VERSION
		, m_defaultAllocatorCallback, m_defaultErrorCallback);

	// pvd connection
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
	//~pvd

	bool recordMemoryAllocations = true;
	physx::PxTolerancesScale scale;
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
	m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, params);
	if (!m_cooking)
		return false;

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
	if (!sceneDesc.gpuDispatcher && m_cudaContextManager)
		sceneDesc.gpuDispatcher = m_cudaContextManager->getGpuDispatcher();

	//sceneDesc.frictionType = physx::PxFrictionType::eTWO_DIRECTIONAL;
	//sceneDesc.frictionType = physx::PxFrictionType::eONE_DIRECTIONAL;
	//sceneDesc.flags |= physx::PxSceneFlag::eENABLE_AVERAGE_POINT;
	//sceneDesc.flags |= physx::PxSceneFlag::eADAPTIVE_FORCE;
	//sceneDesc.flags |= physx::PxSceneFlag::eDISABLE_CONTACT_CACHE;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
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

	physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	return true;
}


bool cPhysicsEngine::PreUpdate(const float deltaSeconds)
{
	physx::PxSceneWriteLock writeLock(*m_scene);
	m_scene->simulate(0.01f, nullptr);
	return true;
}


// update physx result info
bool cPhysicsEngine::PostUpdate(const float deltaSeconds)
{
	PxSceneWriteLock writeLock(*m_scene);
	m_scene->fetchResults(true);
	if (m_objSync)
		m_objSync->Sync();
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


void cPhysicsEngine::Clear()
{
	SAFE_DELETE(m_objSync);
	if (m_physics)
		m_physics->unregisterDeletionListener(*this);
	PHY_SAFE_RELEASE(m_scene);
	PHY_SAFE_RELEASE(m_cudaContextManager);
	PHY_SAFE_RELEASE(m_cpuDispatcher);
	PHY_SAFE_RELEASE(m_cooking);
	PHY_SAFE_RELEASE(m_material);
	PHY_SAFE_RELEASE(m_physics);
	PHY_SAFE_RELEASE(m_pvd);
	PHY_SAFE_RELEASE(m_transport);
	PHY_SAFE_RELEASE(m_foundation);
}
