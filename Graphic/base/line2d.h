// 
// 2D Line을 그린다.
//
#pragma once


namespace graphic
{

	class cLine2d
	{
	public:
		cLine2d(const float width=1);
		virtual ~cLine2d();

		void Render(cRenderer &renderer);
		void Render(cRenderer &renderer, const Vector2 *vList, const int vListCount);
		void SetLine(const Vector2 &p0, const Vector2 &p1);


	public:
		float m_width;
		DWORD m_color;		// D3DCOLOR_XRGB(0, 0, 255) 설정
		Vector2 m_vList[2];
		LPD3DXLINE m_dxLine;
	};

}
