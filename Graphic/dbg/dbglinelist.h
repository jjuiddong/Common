//
// 2017-06-20, jjuiddong
// debug line strip
// Vertex = Position + Diffuse
//
#pragma once


namespace graphic
{
	
	class cDbgLineList
	{
	public:
		cDbgLineList();
		virtual ~cDbgLineList();

		bool Create(cRenderer &renderer, const int maxLines, const DWORD color=0);
		bool AddLine(const Vector3 &p0, const Vector3 &p1);
		void SetColor(const DWORD color);
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		void ClearLines();


	public:
		cVertexBuffer m_vtxBuff;
		int m_lineCount;
		int m_maxLineCount;
		DWORD m_color;
	};

}
