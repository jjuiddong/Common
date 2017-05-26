//
// 2017-05-26, jjuiddong
// Bounding Sphere
//
#pragma once


namespace graphic
{

	class cBoundingSphere
	{
	public:
		cBoundingSphere() : 
			m_pos(0,0,0), m_radius(0) {
		}
		cBoundingSphere(const Vector3 &pos, const float radius) :
			m_pos(pos), m_radius(radius) {
		}
		virtual ~cBoundingSphere() {
		}

		void Set(const cBoundingBox &bbox, const Matrix44 &tm=Matrix44::Identity) {
			const Vector3 v0 = bbox.m_max * tm;
			const Vector3 v1 = bbox.m_min * tm;
			m_pos = (v0 + v1) * 0.5f;
			m_radius = (v0 - v1).Length() * 0.5f;
		}

		cBoundingSphere operator * (const Matrix44 &rhs) {
			return cBoundingSphere(m_pos*rhs, m_radius);
		}

		const cBoundingSphere& operator *= (const Matrix44 &rhs) {
			m_pos *= rhs;
			return *this;
		}


	public:
		Vector3 m_pos;
		float m_radius;
	};

}
