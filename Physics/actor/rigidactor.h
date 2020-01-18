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

	class cRigidActor
	{
	public:
		enum class eType { Static, Dynamic, None };
		enum class eShape { Plane, Box, Sphere, Capsule, Convex, None };

		cRigidActor();
		virtual ~cRigidActor();
		
		bool CreatePlane(cPhysicsEngine &physics
			, const Vector3 &norm);

		bool CreateBox(cPhysicsEngine &physics
			, const Vector3& pos
			, const Vector3& dims
			, const Vector3* linVel = nullptr
			, float density = 1.f);

		bool CreateSphere(cPhysicsEngine &physics
			, const Vector3& pos
			, const float radius
			, const Vector3* linVel = nullptr
			, float density = 1.f);

		bool  CreateCapsule(cPhysicsEngine &physics
			, const Vector3& pos
			, const float radius
			, const float halfHeight
			, const Vector3* linVel = nullptr
			, float density = 1.f);

		void Clear();


	public:
		eType m_type;
		eShape m_shape;
		physx::PxRigidActor *m_actor;
	};

}
