//
// 2020-01-18, jjuiddong
// physics sync class
//		- physics actor, render actor synchronize
//
#pragma once


namespace phys
{

	struct sSyncInfo 
	{
		int id; // unique id
		bool isRemove; // auto remove?
		Str32 name;
		cRigidActor *actor; // physics object
		cJoint *joint; // physics object
		graphic::cNode *node; // render object
	};


	class cPhysicsSync : public iPhysicsSync
	{
	public:
		cPhysicsSync();
		virtual ~cPhysicsSync();

		bool Create(cPhysicsEngine *physics);

		int SpawnPlane(graphic::cRenderer &renderer
			, const Vector3& norm
			, const Str32 &name = "plane");

		int SpawnBox(graphic::cRenderer &renderer
			, const Transform& tfm
			, const float density = 1.f
			, const bool isKinematic = false
			, const Str32 &name = "box");

		int SpawnSphere(graphic::cRenderer &renderer
			, const Transform &tfm
			, const float radius
			, const float density = 1.f
			, const bool isKinematic = false
			, const Str32 &name = "sphere");

		int SpawnCapsule(graphic::cRenderer &renderer
			, const Transform& tfm
			, const float radius
			, const float halfHeight
			, const float density = 1.f
			, const bool isKinematic = false
			, const Str32 &name = "capsule");

		bool AddJoint(cJoint *joint, graphic::cNode *node = nullptr
			, const bool isAutoRemove=true);

		sSyncInfo* FindSyncInfo(const int syncId);
		sSyncInfo* FindSyncInfo(const cRigidActor *actor);
		sSyncInfo* FindSyncInfo(const cJoint *joint);
		bool RemoveSyncInfo(const int syncId);
		bool RemoveSyncInfo(const cRigidActor *actor);
		bool RemoveSyncInfo(const cJoint *joint);
		bool RemoveSyncInfo(sSyncInfo *sync);
		void Clear();


	protected:
		// iPhysicsSync interface
		virtual bool Sync() override;

		// PxDeletionListener interface
		virtual void onRelease(const physx::PxBase* observed, void* userData
			, physx::PxDeletionEventFlag::Enum deletionEvent) override;


	public:
		cPhysicsEngine *m_physics; // reference
		uint m_activeBufferCapacity;
		physx::PxActiveTransform *m_bufferedActiveTransforms;
		vector<sSyncInfo*> m_syncs;
	};

}
