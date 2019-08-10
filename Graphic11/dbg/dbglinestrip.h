//
// 2019-08-10, jjuiddong
// debug line strip
// Vertex = Position + Diffuse
//
#pragma once


namespace graphic
{
	using namespace common;

	class cDbgLineStrip
	{
	public:
		cDbgLineStrip();
		virtual ~cDbgLineStrip();

		bool Create(cRenderer &renderer, const int maxPoint, const cColor &color = cColor::BLACK);
		bool AddPoint(cRenderer &renderer, const Vector3 &pos
			, const bool isUpdateBuffer = true);
		void UpdateBuffer(cRenderer &renderer);
		void Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);
		void ClearLines();


	public:
		cVertexBuffer m_vtxBuff;
		vector<Vector3> m_points;
		int m_maxPointCount;
		uint m_pointCount;
		cColor m_color;
	};

}
