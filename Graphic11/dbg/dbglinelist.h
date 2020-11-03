//
// 2017-06-20, jjuiddong
// debug line strip
// Vertex = Position + Diffuse
//
// 2017-08-07
//	- Upgrade DX11
//
#pragma once


namespace graphic
{
	using namespace common;

	class cDbgLineList
	{
	public:
		cDbgLineList();
		virtual ~cDbgLineList();

		bool Create(cRenderer &renderer, const uint maxLines, const cColor &color=cColor::BLACK);
		bool AddLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1
			, const bool isUpdateBuffer = true);
		bool AddNextPoint(cRenderer &renderer, const Vector3 &p0
			, const bool isUpdateBuffer = true);
		void UpdateBuffer(cRenderer &renderer);
		void Render(cRenderer &renderer, const XMMATRIX &tm=XMIdentity);
		void ClearLines();
		void Clear();


	public:
		cVertexBuffer m_vtxBuff;
		vector<std::pair<Vector3,Vector3>> m_lines;
		uint m_lineCount;
		cColor m_color;
	};

}
