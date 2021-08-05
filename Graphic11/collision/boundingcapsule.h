//
// 2021-08-03, jjuiddong
// Bounding Capsule
//	- capsule collision
//
//------------
//     /\       radius
//    /  \
//------------
//    |  |      half height
//------------
//    |  |
//------------
//    \  /
//     \/
//------------
//
// total height = (radius + m_line.len) x 2
//
#pragma once


namespace common
{

	class cBoundingCapsule : public cCollisionObj
	{
	public:
		cBoundingCapsule(const float radius = 1.0f, const float halfHeight = 1.0f);
		virtual ~cBoundingCapsule();

		virtual bool Collision(const cCollisionObj &obj
			, OUT Vector3 *outPos = NULL, OUT float *distance = NULL) const override;
		virtual bool Pick(const Ray &ray, OUT float *distance = NULL) const override;
		virtual Transform GetTransform() const override;

		bool Intersects(const cBoundingCapsule &bcapsule) const;
		bool Intersects(const cBoundingSphere &bsphere) const;

		void SetCapsule(const Vector3 &pos0, const Vector3 &pos1, const float radius);
		void SetPos(const Vector3 &pos);
		const Vector3& GetPos() const;


	public:
		float m_radius;
		Line m_line;
	};

}
