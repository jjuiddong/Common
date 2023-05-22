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
			, OUT Vector3 *outPos = nullptr
			, OUT float *distance = nullptr
			, OUT bool* isContain = nullptr) const override;
		virtual bool Pick(const Ray &ray, OUT float *distance = NULL) const override;
		virtual Transform GetTransform() const override;

		bool Intersects(const cBoundingCapsule &bcapsule, float *outGap = nullptr) const;
		bool Intersects(const cBoundingSphere &bsphere, float *outGap = nullptr) const;

		void SetCapsule(const Vector3 &pos0, const Vector3 &pos1, const float radius);
		void SetCapsule(const Vector3 &center, const Vector3 &dir, const float halfLen
			, const float radius);
		void SetCapsuleBox(const Vector3& center, const Vector3& dir
			, const float forwardLen, const float sideLen);
		void SetPos(const Vector3 &pos);
		const Vector3& GetPos() const;


	public:
		float m_radius;
		Line m_line;
	};

}
