//
// 2017-05-26, jjuiddong
// Bounding Sphere
//
//	BoundingSphere
//		- XMFLOAT3 Center;            // Center of the sphere.
//		- float Radius;               // Radius of the sphere.
//
#pragma once


namespace graphic
{

	class cBoundingSphere
	{
	public:
		cBoundingSphere() {
		}
		cBoundingSphere(const Vector3 &center, const float radius) :
			m_bsphere(*(XMFLOAT3*)&center, radius) {
		}
		virtual ~cBoundingSphere() {
		}

		void SetBoundingSphere(const Vector3 &center, const float radius) {
			m_bsphere.Center = *(XMFLOAT3*)&center;
			m_bsphere.Radius = radius;
		}

		void SetBoundingSphere(const cBoundingBox &bbox, const Matrix44 &tm=Matrix44::Identity) {
			BoundingSphere::CreateFromBoundingBox(m_bsphere, bbox.m_bbox);
		}

		void SetPos(const Vector3 &pos) {
			m_bsphere.Center = *(XMFLOAT3*)&pos;
		}

		const Vector3& GetPos() const {
			return *(Vector3*)&m_bsphere.Center;
		}

		void SetRadius(const float radius) {
			m_bsphere.Radius = radius;
		}

		float GetRadius() const {
			return m_bsphere.Radius;
		}

		bool Intersects(const Ray &ray, OUT float *distance = NULL ) const {
			const XMVECTOR dir = XMLoadFloat3((XMFLOAT3*)&ray.dir);
			const XMVECTOR orig = XMLoadFloat3((XMFLOAT3*)&ray.orig);
			float dist;
			const bool result = m_bsphere.Intersects(orig, dir, dist);
			if (distance)
				*distance = dist;
			return result;
		}

		bool Intersects(const cBoundingSphere &bspere) const {
			return m_bsphere.Intersects(bspere.m_bsphere);
		}

		cBoundingSphere operator * (const Transform &rhs) {
			Matrix44 m;
			m.SetScale(rhs.scale);
			const Vector3 radius = Vector3(m_bsphere.Radius, 0, 0) * m;
			return cBoundingSphere(*(Vector3*)&m_bsphere.Center * rhs, radius.x);
		}

		cBoundingSphere operator * (const Matrix44 &rhs) {
			const float scale = rhs.GetScale().Length();
			return cBoundingSphere(*(Vector3*)&m_bsphere.Center * rhs, m_bsphere.Radius*scale);
		}

		cBoundingSphere operator * (const XMMATRIX &rhs) {
			const XMVECTOR center = XMLoadFloat3(&m_bsphere.Center);
			const XMVECTOR pos = XMVector3Transform(center, rhs);
			XMFLOAT3 outPos;
			XMStoreFloat3(&outPos, pos);

			XMVECTOR _t, _s, _q;
			XMMatrixDecompose(&_s, &_q, &_t, rhs);
			//_s = XMVector3Length(_s);

			XMFLOAT3 outScale;
			XMStoreFloat3(&outScale, _s);
			XMMATRIX mScale = XMMatrixScaling(outScale.x, outScale.y, outScale.z);
			XMVECTOR vRadius = XMLoadFloat3((XMFLOAT3*)&Vector3(m_bsphere.Radius, 0, 0));
			const XMVECTOR radius = XMVector3Transform(vRadius, mScale);

			XMFLOAT3 retRadius;
			XMStoreFloat3(&retRadius, radius);

			//return cBoundingSphere(*(Vector3*)&outPos, m_bsphere.Radius*outScale.x);
			return cBoundingSphere(*(Vector3*)&outPos, retRadius.x);
		}

		const cBoundingSphere& operator *= (const Matrix44 &rhs) {
			*(Vector3*)&m_bsphere.Center *= rhs;
			return *this;
		}

		XMMATRIX GetTransformXM() const {
			XMMATRIX tm = XMMatrixScaling(m_bsphere.Radius, m_bsphere.Radius, m_bsphere.Radius)
				* XMMatrixTranslation(m_bsphere.Center.x, m_bsphere.Center.y, m_bsphere.Center.z);
			return tm;
		}


	public:
		BoundingSphere m_bsphere;
	};

}
