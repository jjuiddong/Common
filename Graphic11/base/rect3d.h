//
// 2017-11-10, jjuiddong
// Rect3D 
// 3D Rect Line Drawing Class
//
// m_pos[4]
// 0 ---------- 1
// |            |
// |            |
// |     +      |
// |            |
// |            |
// 3 ---------- 2
//
//
#pragma once


namespace graphic
{
	using namespace common;

	class cRect3D : public cNode
	{
	public:
		cRect3D();
		virtual ~cRect3D();

		bool Create(cRenderer &renderer, const int maxEdgeLineCount = 32);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1);
		bool SetRect(cRenderer &renderer, const cRect3D &rect3D);
		bool SetRect(cRenderer &renderer, const Vector3 pos[4]);
		bool SetRect(cRenderer &renderer, const vector<Vector3> &lines);
		bool SetRect(cRenderer &renderer, const Vector3 lines[], const int size);
		void SetColor(const cColor &color);


	public:
		cVertexBuffer m_vtxBuff;
		int m_lineCount;
		cColor m_color; // default: BLACK
		int m_rectId; // user define id
		Vector3 m_pos[4]; // left-top, right-top, right-bottom, left-bottom (X-Z Plane)
	};


	inline void cRect3D::SetColor(const cColor &color) { m_color = color; }
}
