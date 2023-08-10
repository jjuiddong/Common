// 
// 2017-08-02, jjuiddong
// OBB (Oriendted Bounding Box)
//
//
//	BoundingOrientedBox
//		- XMFLOAT3 Center;            // Center of the box.
//		- XMFLOAT3 Extents;           // Distance from the center to each side.
//		- XMFLOAT4 Orientation;       // Unit quaternion representing rotation (box -> world).
//
#pragma once


namespace common
{
	class cBoundingSphere;

	class cBoundingBox : public cCollisionObj
	{
	public:
		cBoundingBox();
		cBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q);
		void SetBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q);
		void SetBoundingBox(const Transform &tfm);
		void SetBoundingBox(const sMinMax &minMax );
		void SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width);
		bool Collision2D(cBoundingSphere &sphere
			, OUT Vector3 *outPos =nullptr
			, OUT Plane *outPlane = nullptr
			, OUT Vector3 *outVertex1 = nullptr
			, OUT Vector3 *outVertex2 = nullptr
		);

		virtual bool Collision(const cCollisionObj &obj
			, OUT Vector3 *outPos = nullptr
			, OUT float *distance = nullptr
			, OUT bool *isContain = nullptr
		) const override;
		virtual bool Pick(const Ray &ray, OUT float *distance = nullptr) const override;
		bool Pick(const Vector3 &orig, const Vector3 &dir, OUT float *distance= nullptr) const;

		Vector3 Center() const;
		void SetCenter(const Vector3 &pos);
		void Scale(const Vector3 &scale);
		float GetScale() const;
		Vector3 GetExtents() const;
		Vector3 GetDimension() const;
		Quaternion GetRotation() const;
		XMMATRIX GetMatrixXM() const;
		Matrix44 GetMatrix() const;
		virtual Transform GetTransform() const override;
		Vector3 GetBoundingPoint(const Vector3 &pos) const;
		void GetVertexPoint(OUT Vector3 out[8]) const;

		cBoundingBox operator + (const cBoundingBox &rhs);
		cBoundingBox operator * (const XMMATRIX &rhs);
		cBoundingBox operator * (const Matrix44 &rhs);
		cBoundingBox& operator += (const cBoundingBox &rhs);
		cBoundingBox& operator *= (const Matrix44 &rhs);
		cBoundingBox& operator *= (const XMMATRIX &rhs);
		//cBoundingBox& operator=(const cCube &cube);


	public:
		BoundingOrientedBox m_bbox;
	};

}
