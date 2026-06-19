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
		int srcId; // linked source object id
		eShapeType type;
		bool isRemove; // auto remove?
		Str32 name;
		Vector3 dim; // box
		float radius; // sphere, cylinder, capsule
		float height; // cylinder, capsule(half height)
		Transform tfm; // sync transform
		cRigidActor *actor; // physics object
		cJoint *j1; // physics object
		cArticulation* art; // articulation
		graphic::cNode *node; // render object, reference
	};


	class cPhysicsSync : public iPhysicsSync
	{
	public:
		cPhysicsSync();
		virtual ~cPhysicsSync();

		bool Create(cPhysicsEngine *physics);

		int SpawnPlane(graphic::cRenderer *renderer
			, const Vector3 &norm
			, const Str32 &name = "plane"
			, const int srcId = 0);

		int SpawnBox(graphic::cRenderer *renderer
			, const Transform &tfm
			, const float density = 1.f
			, const bool isKinematic = false
			, const Str32 &name = "box"
			, const int srcId = 0);

		int SpawnSphere(graphic::cRenderer *renderer
			, const Transform &tfm
			, const float radius
			, const float density = 1.f
			, const bool isKinematic = false
			, const Str32 &name = "sphere"
			, const int srcId = 0);

		int SpawnCapsule(graphic::cRenderer *renderer
			, const Transform &tfm
			, const float radius
			, const float halfHeight
			, const float density = 1.f
			, const bool isKinematic = false
			, const Str32 &name = "capsule"
			, const int srcId = 0);

		int SpawnCylinder(graphic::cRenderer *renderer
			, const Transform &tfm
			, const float radius
			, const float height
			, const float density = 1.f
			, const bool isKinematic = false
			, const Str32 &name = "cylinder"
			, const int srcId = 0);

		bool AddJoint(cJoint *j1, graphic::cNode *node = nullptr
			, const bool isAutoRemove=true);

		bool AddArticulation(cArticulation* art);

		bool AddCompound(sSyncInfo *sync0, sSyncInfo *sync1);

		sSyncInfo* FindSyncInfo(const int syncId);
		sSyncInfo* FindSyncInfo(const cRigidActor *actor);
		sSyncInfo* FindSyncInfo(const cJoint *j1);
		sSyncInfo* FindSyncInfo(const cArticulation* art);
		bool RemoveSyncInfo(const int syncId);
		bool RemoveSyncInfo(const cRigidActor *actor);
		bool RemoveSyncInfo(const cJoint *j1);
		bool RemoveSyncInfo(const cArticulation* art);
		bool RemoveSyncInfo(sSyncInfo *sync);
		void ClearSyncInfo(const bool isClearGroundPlane=true);
		void Clear();


	protected:
		// iPhysicsSync interface
		virtual bool Sync() override;

		// PxDeletionListener interface
		virtual void onRelease(const physx::PxBase* observed, void* userData
			, physx::PxDeletionEventFlag::Enum deletionEvent) override;


	public:
		cPhysicsEngine *m_physics; // reference
		vector<sSyncInfo*> m_syncs;
		map<int, sSyncInfo*> m_syncs2; // fast search, reference, key: sync id
	};

}
