//
// 2016-04-30, jjuiddong
// 원을 선으로 그려준다.
//
#pragma once


namespace graphic
{

	class cCircleLine
	{
	public:
		cCircleLine();
		virtual ~cCircleLine();

		bool Create(cRenderer &renderer, const Vector3 &center, const float radius, const float width, const float start=0, const float last=MATH_PI * 2.f);
		void Render(cRenderer &renderer);
		void MuliplyTransform(const Matrix44 &tm);


	public:
		float m_radius;
		cLine m_lines[20];
	};

}
