//
// 2020-01-18, jjuiddong
// PhysX engine wrapper class
// physics object manager
// 
// 2026-05-23
//	- multi PxScene
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
		bool SceneLockWrite();
		bool SceneUnlockWrite();
		bool Play();
		bool Pause();
		void Clear();
		static void ClearPhysx();


	protected:
		// PxDeletionListener interface override
		virtual void onRelease(const physx::PxBase* observed, void* userData
			, physx::PxDeletionEventFlag::Enum deletionEvent) override;


	public:
		static physx::PxFoundation *s_foundation;
		static physx::PxPhysics *s_physics;
		static physx::PxPvd *s_pvd;
		static physx::PxPvdTransport *s_transport;
		static physx::PxDefaultAllocator s_defaultAllocatorCallback;
		static physx::PxPvdInstrumentationFlags s_pvdFlags;
		static cDefaultErrorCallback s_defaultErrorCallback;

		physx::PxMaterial* m_material;
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
		float m_timerScale; // default: 1.0
	};

}
