//
// 2017-012-24, jjuiddong
// collision object
//
#pragma once


namespace graphic
{
	using namespace common;

	struct eCollisionType {
		enum Enum {
			NONE, BOX, SPHERE, HALFSPHERE, PLANE, FRUSTUM,
		};
	};


	//------------------------------------------------------------------
	// Collision Object
	class cCollisionObj
	{
	public:
		cCollisionObj() : m_type(eCollisionType::NONE) {}
		virtual ~cCollisionObj() {}

		virtual bool Collision(const cCollisionObj &obj
			, OUT Vector3 *outPos = NULL, OUT float *distance = NULL) const = 0;

		virtual bool Pick(const Ray &ray, OUT float *distance = NULL) const = 0;

		virtual Transform GetTransform() const = 0;

		eCollisionType::Enum m_type;
	};

}
