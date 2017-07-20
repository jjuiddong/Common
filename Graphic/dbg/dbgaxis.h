//
// 2017-07-20, jjuiddong
// Render Axis
//
#pragma once


namespace graphic
{

	class cDbgAxis
	{
	public:
		cDbgAxis();
		virtual ~cDbgAxis();

		bool Create(cRenderer &renderer);
		void SetAxis(const float size, const cBoundingBox &bbox);
		void SetAxis(const cBoundingBox &bbox);
		void Render(cRenderer &renderer, const Matrix44 &tm);


	public:
		cDbgLine m_lines[3]; // x-y-z
	};

}
