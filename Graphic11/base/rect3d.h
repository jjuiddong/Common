//
// 2017-11-10, jjuiddong
// Rect3D 
// LineDrawing Rect
//
#pragma once


namespace graphic
{

	class cRect3D : public cNode
	{
	public:
		cRect3D();
		virtual ~cRect3D();

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1);
		void SetRect(const Vector3 pos[4]);


	public:
		Vector3 m_pos[4]; // lefttop, righttop, leftbottom, rightbottom
	};

}