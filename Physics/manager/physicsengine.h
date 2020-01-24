//
// 2020-01-18, jjuiddong
// PhysX engine wrapper class
// physics object manager
//
#pragma once


namespace phys
{
	class cRigidActor;
	class cJoint;
	interface iPhysicsSync;

	class cPhysicsEngine : public physx::PxDeletionListener
	{
	public:
		cPhysicsEngine();
		virtual ~cPhysicsEngine();

		bool InitializePhysx();
		bool PreUpdate(const float deltaSeconds);
		bool PostUpdate(const float deltaSeconds);

		bool SetPhysicsSync(iPhysicsSync *sync);
		void Clear();


	protected:
		// PxDeletionListener interface override
		virtual void onRelease(const physx::PxBase* observed, void* userData
			, physx::PxDeletionEventFlag::Enum deletionEvent) override;


	public:
		physx::PxFoundation *m_foundation;
		physx::PxPhysics *m_physics;
		physx::PxMaterial *m_material;
		physx::PxPvd *m_pvd;
		physx::PxPvdTransport *m_transport;
		physx::PxCooking *m_cooking;
		physx::PxDefaultAllocator m_defaultAllocatorCallback;
		physx::PxPvdInstrumentationFlags m_pvdFlags;
		cDefaultErrorCallback m_defaultErrorCallback;
		physx::PxDefaultCpuDispatcher *m_cpuDispatcher;
		physx::PxCudaContextManager *m_cudaContextManager;
		physx::PxScene *m_scene;
		void *m_scratchBlock;
		uint m_scratchBlockSize;

		iPhysicsSync *m_objSync;
		bool m_isPVD; // pvd connection?

		// stepper
		float m_stepSize;
		float m_accTime;
		bool m_isFetch;
	};

}
