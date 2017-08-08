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
	
	class cDbgLineList
	{
	public:
		cDbgLineList();
		virtual ~cDbgLineList();

		bool Create(cRenderer &renderer, const int maxLines, const cColor &color=cColor::BLACK);
		bool AddLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1);
		bool AddNextPoint(cRenderer &renderer, const Vector3 &p0);
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		void ClearLines();


	protected:
		void UpdateBuffer(cRenderer &renderer);


	public:
		cVertexBuffer m_vtxBuff;
		vector<std::pair<Vector3,Vector3>> m_lines;
		int m_lineCount;
		int m_maxLineCount;
		cColor m_color;
	};

}
