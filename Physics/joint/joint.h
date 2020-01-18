//
// 2020-01-18, jjuiddong
// physics joint
//		- PhysX PxJoint wrapping class
//
#pragma once


namespace phys
{

	class cJoint
	{
	public:
		enum class eType {Fixed, Spherical, Revolute, Prismatic, Distance, D6, None};

		cJoint();
		virtual ~cJoint();

		bool CreateFixedJoint(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0
			, cRigidActor *actor1, const Transform &worldTfm1);

		bool CreateSphericalJoint(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0
			, cRigidActor *actor1, const Transform &worldTfm1);

		void Clear();


	public:
		eType m_type;
		physx::PxJoint *m_joint;
		physx::PxFixedJoint *m_fixedJoint; // reference
		physx::PxSphericalJoint *m_sphericalJoint; // reference
	};

}
