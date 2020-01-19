//
// 2020-01-18, jjuiddong
// physics sync class
//
#pragma once


namespace phys
{

	class cPhysicsSync : public iPhysicsSync
	{
	public:
		struct sActorInfo {
			int id;
			string name;
			cRigidActor *actor;
			graphic::cNode *node;
		};

		cPhysicsSync();
		virtual ~cPhysicsSync();

		bool Create(cPhysicsEngine *physics);

		int SpawnPlane(graphic::cRenderer &renderer
			, const Vector3& norm);

		int SpawnBox(graphic::cRenderer &renderer
			, const Transform& tfm
			, const float density = 1.f);

		int SpawnSphere(graphic::cRenderer &renderer
			, const Vector3& pos
			, const float radius
			, const float density = 1.f);

		int SpawnCapsule(graphic::cRenderer &renderer
			, const Transform& tfm
			, const float radius
			, const float halfHeight
			, const float density = 1.f);

		bool AddJoint(cJoint *joint);
		sActorInfo* FindActorInfo(const int id);
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
		vector<sActorInfo*> m_actors;
		vector<cJoint*> m_joints;
	};

}