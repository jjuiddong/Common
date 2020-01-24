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
			, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
			, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1
			, const Vector3 &revoluteAxis);

		bool ModifyPivot(cPhysicsEngine &physics
			, const Transform &worldTfm0, const Vector3 &pivot0
			, const Transform &worldTfm1, const Vector3 &pivot1
			, const Vector3 &revoluteAxis);

		bool CreateReferenceMode();

		// spherical joint wrapping function
		bool SetLimitCone(const physx::PxJointLimitCone &config);
		bool SetSphericalJointFlag(physx::PxSphericalJointFlag::Enum flag, bool value);

		// revolute joint wrapping function
		bool SetLimit(const physx::PxJointAngularLimitPair &config);
		bool SetRevoluteJointFlag(physx::PxRevoluteJointFlag::Enum flag, bool value);
		bool SetDriveVelocity(const float velocity);

		void Clear();


	protected:
		Vector3 CalcJointPos(const Transform &worldTm0, const Transform &worldTm1);

		void GetLocalFrame(const Transform &worldTm0, const Transform &worldTm1
			, const Vector3 &jointPos, const Vector3 &revoluteAxis
			, OUT physx::PxTransform &out0, OUT physx::PxTransform &out1);

		void GetRelativeActorPos(const Transform &worldTm0, const Transform &worldTm1
			, OUT Vector3 &relPos0, OUT Vector3 &relPos1);


	public:
		int m_id;
		eType m_type;
		bool m_referenceMode; // m_joint is reference, for ui joint renderer
		cRigidActor *m_actor0; // joint pair actor0, reference
		cRigidActor *m_actor1; // joint pair actor1, reference
		physx::PxJoint *m_joint;

		// joint property
		const float m_breakForce = 0.f;// 600.f;
		float m_revoluteAxisLen;
		Vector3 m_revoluteAxis; // local space
		Vector3 m_origPos; // joint origin pos (local space)
		Quaternion m_rotRevolute; // origin direction -> revoluteAxis rotation
								  // origin direction = normal(actor1 - actor0)
		//Vector3 m_toActor0; // joint pos -> actor0 pos (before revolute rotation)
		//Vector3 m_toActor1; // joint pos -> actor1 pos (before revolute rotation)
		//Trasnform m_jointTransform; // transform to joint
		Transform m_actorLocal0; // actor0 local transform (local space)
		Transform m_actorLocal1; // actor0 local transform (local space)
	};

}
