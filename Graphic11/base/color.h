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
		cColor(const float r, const float g, const float b, const float a = 1.f);
		cColor(const BYTE r, const BYTE g, const BYTE b, const BYTE a = 255);
		cColor(const Vector3 &color);
		cColor(const Vector4 &color);

		static const cColor BLACK;
		static const cColor WHITE;

		virtual ~cColor();

		void SetColor(const float r, const float g, const float b, const float a=1.f);
		void SetColor(const BYTE r, const BYTE g, const BYTE b, const BYTE a=255);
		void SetColor(const Vector3 &color);
		void SetColor(const Vector4 &color);
		Vector4 GetColor() const;

		operator DWORD ();
		bool operator == (const cColor &color);
		bool operator != (const cColor &color);


	public:
		DWORD m_color; // ARGB
	};
}
