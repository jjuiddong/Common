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

		//bool Create
		void Clear();


	public:
		eType m_type;
		physx::PxJoint *m_joint;
	};

}
