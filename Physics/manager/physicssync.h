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
		cPhysicsSync();
		virtual ~cPhysicsSync();

		bool Create(cPhysicsEngine *physics);

		graphic::cGridLine* SpawnPlane(graphic::cRenderer &renderer
			, const Vector3& norm);

		graphic::cCube* SpawnBox(graphic::cRenderer &renderer
			, const Vector3& pos
			, const Vector3 &scale);

		graphic::cSphere* SpawnSphere(graphic::cRenderer &renderer
			, const Vector3& pos
			, const float radius);

		graphic::cCapsule* SpawnCapsule(graphic::cRenderer &renderer
			, const Vector3& pos
			, const float radius
			, const float halfHeight);

		bool AddJoint(cJoint *joint);
		void Clear();


	protected:
		// iPhysicsSync interface
		virtual bool Sync() override;

		// PxDeletionListener interface
		virtual void onRelease(const physx::PxBase* observed, void* userData
			, physx::PxDeletionEventFlag::Enum deletionEvent) override;


	public:
		struct sActor {
			int id;
			string name;
			cRigidActor *actor;
			graphic::cNode *node;
		};

		cPhysicsEngine *m_physics; // reference
		uint m_activeBufferCapacity;
		physx::PxActiveTransform *m_bufferedActiveTransforms;
		vector<sActor> m_actors;
		vector<cJoint*> m_joints;
	};

}
