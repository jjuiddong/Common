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

		bool CreateFixed(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0
			, cRigidActor *actor1, const Transform &worldTfm1);

		bool CreateSpherical(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0
			, cRigidActor *actor1, const Transform &worldTfm1);

		bool CreateRevolute(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0
			, cRigidActor *actor1, const Transform &worldTfm1
			, const Vector3 &revoluteAxis);

		void Clear();


	protected:
		void GetLocalFrame(const Transform &worldTm0, const Transform &worldTm1
			, const Vector3 &revoluteAxis
			, OUT physx::PxTransform &out0, OUT physx::PxTransform &out1);


	public:
		eType m_type;
		cRigidActor *m_actor0; // joint pair actor0
		cRigidActor *m_actor1; // joint pair actor1
		physx::PxJoint *m_joint;
		physx::PxFixedJoint *m_fixedJoint; // reference
		physx::PxSphericalJoint *m_sphericalJoint; // reference
		physx::PxRevoluteJoint *m_revoluteJoint; // reference
	};

}
