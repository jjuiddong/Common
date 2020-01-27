//
// 2020-01-18, jjuiddong
// physics joint
//		- PhysX PxJoint wrapping class
//
#pragma once


namespace phys
{

	DECLARE_ENUM(eJointType, Fixed, Spherical, Revolute, Prismatic, Distance, D6, None);


	class cJoint
	{
	public:

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

		bool Update(const float deltaSeconds);

		// joint pivot
		void SetPivotPos(const int actorIndex, const Vector3 &pos);
		Transform GetPivotWorldTransform(const int actorIndex);
		Vector3 GetPivotPos(const int actorIndex);

		// spherical joint wrapping function
		bool SetSphericalJointFlag(physx::PxSphericalJointFlag::Enum flag, bool value);
		bool EnableConeLimit(const bool enable);
		bool IsConeLimit();
		bool SetConeLimit(const physx::PxJointLimitCone &config);
		physx::PxJointLimitCone GetConeLimit();

		// revolute joint wrapping function
		bool SetRevoluteJointFlag(physx::PxRevoluteJointFlag::Enum flag, bool value);
		bool EnableDrive(const bool enable);
		bool IsDrive();
		bool SetDriveVelocity(const float velocity);
		float GetDriveVelocity();

		bool EnableAngularLimit(const bool enable);
		bool IsAngularLimit();
		bool SetAngularLimit(const physx::PxJointAngularLimitPair &config);
		physx::PxJointAngularLimitPair GetAngularLimit();

		bool IsCycleDrive();
		bool EnableCycleDrive(const bool enable);
		bool SetCycleDrivePeriod(const float period, const float accel);
		Vector2 GetCycleDrivePeriod();

		void Clear();


	protected:
		void DefaultJointConfiguration(physx::PxJoint *joint);

		void GetLocalFrame(const Transform &worldTm0, const Transform &worldTm1
			, const Vector3 &jointPos, const Vector3 &revoluteAxis
			, OUT physx::PxTransform &out0, OUT physx::PxTransform &out1);


	public:
		int m_id; // unique id
		eJointType::Enum m_type;
		bool m_referenceMode; // m_joint is reference, for ui joint renderer
		cRigidActor *m_actor0; // joint pair actor0, reference
		cRigidActor *m_actor1; // joint pair actor1, reference
		physx::PxJoint *m_joint;

		// drive cycle period (revolute joint)
		bool m_isCycleDrive;
		float m_incT; // increment time
		float m_incAccelT; // increment acceleration time
		float m_cyclePeriod; // period, seconds unit
		float m_curVelocity; // current drive velocity
		float m_cycleDriveAccel; // drive velocity acceleration
		float m_maxDriveVelocity; // maximum drive velocity
		bool m_toggleDir;

		// joint property
		const float m_breakForce;
		float m_revoluteAxisLen;
		Vector3 m_revoluteAxis; // local space
		Vector3 m_origPos; // joint origin pos (local space)
		Quaternion m_rotRevolute; // Xaxis -> revoluteAxis rotation (local space)
								  // revoluteAxis = normal(pivot1 - pivot0)
		Transform m_actorLocal0; // actor0 local transform (local space)
		Transform m_actorLocal1; // actor0 local transform (local space)

		struct sPivot 
		{
			Vector3 dir; // pivot direction(local space), actor -> pivot
			float len; // pivot length
		};
		sPivot m_pivots[2]; // actor0,1
	};

}
