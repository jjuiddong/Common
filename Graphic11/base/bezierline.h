//
// 2019-07-10, jjuiddong
// Bezier Line 
//
#pragma once


namespace graphic
{

	class cBezierLine : public cNode
	{
	public:
		cBezierLine();
		virtual ~cBezierLine();

		virtual bool Create(cRenderer &renderer, const uint slice, const cColor &color);
		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		void SetLine(const Vector3 &p0, const Vector3 &p1, 
			const Vector3 &p2, const Vector3 &p3, const float width);
		void SetLine(const Vector3 p[4], const float width);
		void SetLine(const Vector3 &p0, const Vector3 &p1, const float height, const float width);


	public:
		uint m_slice;
		cLine m_line;
		cCubeShape m_shape;
		cColor m_color;
		Vector3 m_points[4];
		vector<Matrix44> m_pts;
	};

}
