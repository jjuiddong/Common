//
// 2025-01-21, jjuiddong
// physx articulation wrapping class
//
#pragma once


namespace phys
{

	class cArticulation
	{
	public:
		cArticulation();
		virtual ~cArticulation();

		bool Create(cPhysicsEngine& physics
			, const bool isFixed = true
			, const int solverIterationCount = 32
		);

		int AddBoxLink(cPhysicsEngine& physics
			, const int parentLinkId
			, const Transform &tfm
			, const float mass = 1.f
			, physx::PxMaterial *material = nullptr
		);

		int AddSphereLink(cPhysicsEngine& physics
			, const int parentLinkId
			, const Transform& tfm
			, const float radius
			, const float mass = 1.f
			, physx::PxMaterial* material = nullptr
		);

		int AddCapsuleLink(cPhysicsEngine& physics
			, const int parentLinkId
			, const Transform& tfm
			, const float radius
			, const float halfHeight
			, const float mass = 1.f
			, physx::PxMaterial* material = nullptr
		);

		int AddCylinderLink(cPhysicsEngine& physics
			, const int parentLinkId
			, const Transform& tfm
			, const float radius
			, const float height
			, const float mass = 1.f
			, physx::PxMaterial* material = nullptr
		);

		physx::PxArticulationJointReducedCoordinate* AddJoint(
			const int childLinkId
			, const eJointType jointType
			, const Vector3& parentPivot0
			, const Vector3& childPivot1
			, const Vector3 axis = Vector3::Zeroes
		);

		physx::PxD6Joint* AddD6Joint(
			cPhysicsEngine& physics
			, const int linkId0, const Vector3 &pivot0
			, const int linkId1, const Vector3 &pivot1
		);

		bool AddScene(cPhysicsEngine& physics);

		physx::PxArticulationLink* GetLink(const int linkId);

		bool SetAttribute(const float linearDamping, const float angularDamping
			, const float maxLinearVelocity, const float maxAngularVelocity);

		void Clear();


	protected:
		physx::PxConvexMesh* GenerateCylinderMesh(cPhysicsEngine& physics
			, const float radius, const float height);

		void GetLocalFrame(const Transform& worldTm0, const Transform& worldTm1
			, const Vector3& jointPos, const Vector3& revoluteAxis
			, OUT physx::PxTransform& out0, OUT physx::PxTransform& out1);


	public:
		physx::PxArticulationReducedCoordinate* m_art;
		map<int, physx::PxArticulationLink*> m_links; // reference, key:link id
	};

}
