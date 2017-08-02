//
// 2017-05-13, jjuiddong
// Debug Box 
//		- Vertex = Position + Diffuse
//		- Triangle Render
//
// 2017-08-02, jjuiddong
// Upgrad DX9 - DX11
//		- BoundingBox
//
//
#pragma once


namespace graphic
{
	class cRenderer;

	class cDbgBox
	{
	public:
		cDbgBox();
		cDbgBox(cRenderer &renderer, const cBoundingBox &bbox, const cColor &color=cColor::BLACK);

		void Create(cRenderer &renderer, const cColor &color = cColor::BLACK);
		void Create(cRenderer &renderer, const cBoundingBox &bbox, const cColor &color = cColor::BLACK);
		void SetBox(const cBoundingBox &bbox);
		void SetColor(DWORD color);
		void Render(cRenderer &renderer, const XMMATRIX &tm=XMIdentity);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		cBoundingBox m_boundingBox;
	};

}
