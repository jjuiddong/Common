//
// 2017-10-25, jjuiddong
// Bounding Half Sphere
//
//	BoundingSphere
//		- XMFLOAT3 Center;            // Ground Center of the half sphere.
//		- float Radius;               // Radius of the sphere.
//
#pragma once


namespace graphic
{

	class cBoundingHalfSphere
	{
	public:
		cBoundingHalfSphere() {
		}
		cBoundingHalfSphere(const Vector3 &center, const float radius) :
			m_center(center), m_radius(radius) {			
		}
		virtual ~cBoundingHalfSphere() {
		}

		void SetBoundingHalfSphere(const Vector3 &center, const float radius) {
			m_center = center;
			m_radius = radius;
		}

		void SetBoundingHalfSphere(const cBoundingBox &bbox, const Matrix44 &tm = Matrix44::Identity) {
			assert(0);
		}

		void SetPos(const Vector3 &pos) {
			m_center = pos;
		}

		const Vector3& GetPos() const {
			return m_center;
		}

		void SetRadius(const float radius) {
			m_radius = radius;
		}

		float GetRadius() const {
			return m_radius;
		}

		bool Intersects(const cBoundingSphere &bspere) {
			assert(0);
			return true;
		}

		Vector3 GetBoundingPoint(const Vector3 &pos) {
			Vector3 v = pos - m_center;
			const float len = min(m_radius, v.Length());
			Vector3 ret = m_center + v.Normal() * len;
			ret.y = max(m_center.y, ret.y);
			return ret;
		}

		cBoundingHalfSphere operator * (const Matrix44 &rhs) {
			const float scale = rhs.GetScale().Length();
			return cBoundingHalfSphere(m_center * rhs, m_radius * scale);
		}

		const cBoundingHalfSphere& operator *= (const Matrix44 &rhs) {
			*this = operator*(rhs);
			return *this;
		}


	public:
		Vector3 m_center;
		float m_radius;
	};

}
