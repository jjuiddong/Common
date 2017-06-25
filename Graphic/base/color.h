//
// 2017-06-25, jjuiddong
// color
//
#pragma once


namespace graphic
{

	class cColor
	{
	public:
		cColor();
		virtual ~cColor();

		void SetColor(const float r, const float g, const float b, const float a=1.f);
		void SetColor(const BYTE r, const BYTE g, const BYTE b, const BYTE a=255);
		void SetColor(const Vector3 &color);
		void SetColor(const Vector4 &color);


	public:
		DWORD m_color;
	};
}
