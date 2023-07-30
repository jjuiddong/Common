//
// 2017-012-24, jjuiddong
// collision object
//
#pragma once


namespace common
{

	struct eCollisionType {
		enum Enum {
			NONE, BOX, SPHERE, HALFSPHERE, PLANE, FRUSTUM, CAPSULE,
		};
	};


	//------------------------------------------------------------------
	// Collision Object
	class cCollisionObj
	{
	public:
		cCollisionObj(const eCollisionType::Enum type = eCollisionType::NONE) 
			: m_type(type) {}
		virtual ~cCollisionObj() {}

		virtual bool Collision(const cCollisionObj &obj
			, OUT Vector3 *outPos = nullptr
			, OUT float *distance = nullptr
			, OUT bool* isContain = nullptr
		) const = 0;

		virtual bool Pick(const Ray &ray, OUT float *distance = NULL) const = 0;

		virtual Transform GetTransform() const = 0;

		eCollisionType::Enum m_type;
	};

}
