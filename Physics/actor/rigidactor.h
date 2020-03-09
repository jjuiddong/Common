//
// 2020-01-18, jjuiddong
// rigid actor
//		- PhysX PxRigidActor wrapping class
//		- PhysX class hierarchy
//			- https://gameworksdocs.nvidia.com/PhysX/3.4/PhysXAPI/files/classPxRigidActor.html
//
#pragma once


namespace phys
{
	class cPhysicsEngine;
	class cJoint;

	DECLARE_ENUM(eRigidType, Static, Dynamic, None);
	DECLARE_ENUM(eShapeType, Plane, Box, Sphere, Capsule, Cylinder, Convex, None);

	class cRigidActor
	{
	public:
		cRigidActor();
		virtual ~cRigidActor();
		
		bool CreatePlane(cPhysicsEngine &physics
			, const Vector3 &norm
			, graphic::cNode *node = nullptr);

		bool CreateBox(cPhysicsEngine &physics
			, const Transform &tfm
			, const Vector3* linVel = nullptr
			, const float density = 1.f
			, const bool isKinematic = false
			, graphic::cNode *node = nullptr);

		bool CreateSphere(cPhysicsEngine &physics
			, const Transform &tfm
			, const float radius
			, const Vector3* linVel = nullptr
			, const float density = 1.f
			, const bool isKinematic = false
			, graphic::cNode *node = nullptr);

		bool CreateCapsule(cPhysicsEngine &physics
			, const Transform &tfm
			, const float radius
			, const float halfHeight
			, const Vector3* linVel = nullptr
			, const float density = 1.f
			, const bool isKinematic = false
			, graphic::cNode *node = nullptr);

		bool CreateCylinder(cPhysicsEngine &physics
			, const Transform &tfm
			, const float radius
			, const float height
			, const Vector3* linVel = nullptr
			, const float density = 1.f
			, const bool isKinematic = false
			, graphic::cNode *node = nullptr);

		bool ChangeDimension(cPhysicsEngine &physics, const Vector3 &dim);

		bool Compound(cPhysicsEngine &physics, cRigidActor *src);

		// wrapping function
		bool SetGlobalPose(const physx::PxTransform &tfm);
		bool SetGlobalPose(const Transform &tfm);
		Transform GetGlobalPose();
		bool SetKinematic(const bool isKinematic);
		bool IsKinematic() const;	
		float GetMass() const;
		bool SetMass(const float mass);
		float GetLinearDamping() const;
		bool SetLinearDamping(const float damping);
		float GetAngularDamping() const;
		bool SetAngularDamping(const float damping);
		bool SetMaxAngularVelocity(const float maxVel);
		float GetMaxAngularVelocity();
		bool SetLinearVelocity(const Vector3 velocity);
		bool SetAngularVelocity(const Vector3 velocity);
		bool ClearForce();
		bool WakeUp();
		bool AddJoint(cJoint *joint);
		bool RemoveJoint(cJoint *joint);
		void Clear();


	protected:
		physx::PxConvexMesh* GenerateCylinderMesh(cPhysicsEngine &physics
			, const float radius, const float height);

		void DefaultRigidActorConfiguration(physx::PxRigidDynamic *actor);


	public:
		int m_id;
		eRigidType::Enum m_type;
		eShapeType::Enum m_shape;
		physx::PxRigidActor *m_actor;
		vector<cJoint*> m_joints; // reference

		struct sMesh
		{
			eShapeType::Enum shape;
			graphic::cNode *node; // graphic node
			Matrix44 local; // relative transform nodes[0] to current
		};
		vector<sMesh> m_meshes;
	};

}
