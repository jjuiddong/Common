//
// 2017-12-25, jjuiddong
// bounding plane
//	- composite 2 triangle
//	- 3D Rectangle
//
// m_vertices[4], clock wise
//  0 ----------- 1
//  | \           |
//  |   \         |
//  |      +      |
//  |        \    |
//  |          \  |
//  3-------------2
//
#pragma once


namespace common
{

	class cBoundingPlane : public cCollisionObj
	{
	public:
		cBoundingPlane();
		cBoundingPlane(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
		virtual ~cBoundingPlane();
		
		void SetVertex(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
		virtual bool Collision(const cCollisionObj &obj
			, OUT Vector3 *pos = NULL, OUT float *distance = NULL) const override;
		virtual bool Pick(const Ray &ray, OUT float *distance = NULL) const override;
		bool Intersect(const Ray &ray, const float radius, OUT float *distance = NULL
			, const int recursiveCnt = 0
		) const;
		virtual Transform GetTransform() const override;
		Vector3 Normal() const;


	public:
		Vector3 m_vertices[4];
		Plane m_plane;
	};

}
