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

	class cBoundingSphere : public cCollisionObj
	{
	public:
		cBoundingSphere();
		cBoundingSphere(const Vector3 &center, const float radius);
		virtual ~cBoundingSphere();

		virtual bool Collision(const cCollisionObj &obj
			, OUT Vector3 *outPos = NULL, OUT float *distance = NULL ) const override;
		virtual bool Pick(const Ray &ray, OUT float *distance = NULL) const override;

		bool Intersects(const Ray &ray, OUT float *distance = NULL) const;
		bool Intersects(const cBoundingSphere &bspere) const;

		void SetBoundingSphere(const Vector3 &center, const float radius);
		void SetBoundingSphere(const cBoundingBox &bbox, const Matrix44 &tm = Matrix44::Identity);
		void SetPos(const Vector3 &pos);
		const Vector3& GetPos() const;
		void SetRadius(const float radius);
		float GetRadius() const;
		virtual Transform GetTransform() const override;
		XMMATRIX GetTransformXM() const;

		cBoundingSphere operator * (const Transform &rhs);
		cBoundingSphere operator * (const Matrix44 &rhs);
		cBoundingSphere operator * (const XMMATRIX &rhs);
		const cBoundingSphere& operator *= (const Matrix44 &rhs);


	public:
		BoundingSphere m_bsphere;
	};

}
