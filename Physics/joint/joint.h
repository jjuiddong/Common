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
			, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
			, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1);

		bool CreateSpherical(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
			, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1);

		bool CreateRevolute(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
			, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1
			, const Vector3 &revoluteAxis);

		bool CreatePrismatic(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
			, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1
			, const Vector3 &revoluteAxis);

		bool CreateDistance(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
			, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1);

		bool CreateD6(cPhysicsEngine &physics
			, cRigidActor *actor0, const Transform &worldTfm0, const Vector3 &pivot0
			, cRigidActor *actor1, const Transform &worldTfm1, const Vector3 &pivot1);

		bool ModifyPivot(cPhysicsEngine &physics
			, const Transform &worldTfm0, const Vector3 &pivot0
			, const Transform &worldTfm1, const Vector3 &pivot1
			, const Vector3 &revoluteAxis);

		bool CreateReferenceMode();

		bool Update(const float deltaSeconds);

		bool ReconnectBreakJoint(cPhysicsEngine &physics);

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

		// prismatic joint wrapping function
		bool EnableLinearLimit(const bool enable);
		bool IsLinearLimit();
		bool SetLinearLimit(const physx::PxJointLinearLimitPair &config);
		physx::PxJointLinearLimitPair GetLinearLimit();

		// distance joint wrapping function
		bool EnableDistanceLimit(const bool enable);
		bool IsDistanceLimit();
		bool SetDistanceLimit(const float minDist, const float maxDist);
		Vector2 GetDistanceLimit();

		// D6 joint wrapping function
		bool SetMotion(const physx::PxD6Axis::Enum axis, const physx::PxD6Motion::Enum motion);
		physx::PxD6Motion::Enum GetMotion(const physx::PxD6Axis::Enum axis);
		bool SetD6Drive(const physx::PxD6Drive::Enum axis, const physx::PxD6JointDrive &drive);
		physx::PxD6JointDrive GetD6Drive(const physx::PxD6Drive::Enum axis);
		bool SetD6DriveVelocity(const Vector3 &linear, const Vector3 &angular);
		std::pair<Vector3,Vector3> GetD6DriveVelocity();
		bool SetD6LinearLimit(const physx::PxJointLinearLimit &config);
		bool SetD6TwistLimit(const physx::PxJointAngularLimitPair &config);
		bool SetD6SwingLimit(const physx::PxJointLimitCone &config);
		physx::PxJointLinearLimit GetD6LinearLimit();
		physx::PxJointAngularLimitPair GetD6TwistLimit();
		physx::PxJointLimitCone GetD6SwingLimit();

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
		bool m_isBroken;
		float m_revoluteAxisLen; // revolute, prismatic joint axis
		Vector3 m_revoluteAxis; // local space
		Vector3 m_origPos; // joint origin pos (local space)
		Quaternion m_rotRevolute; // Xaxis -> revoluteAxis rotation (local space)
								  // revoluteAxis = normal(pivot1 - pivot0)
		Transform m_actorLocal0; // actor0 local transform (local space)
		Transform m_actorLocal1; // actor1 local transform (local space)

		struct sPivot 
		{
			Vector3 dir; // pivot direction(local space), actor -> pivot
			float len; // pivot length
		};
		sPivot m_pivots[2]; // actor0,1
	};

}
