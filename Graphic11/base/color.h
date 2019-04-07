//
// 2017-06-25, jjuiddong
// color
//
#pragma once


#define COLOR_ARGB(a,r,g,b) \
    ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define COLOR_RGBA(r,g,b,a) COLOR_ARGB(a,r,g,b)

#define COLOR_COLORVALUE(r,g,b,a) \
    COLOR_RGBA((DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f),(DWORD)((a)*255.f))


namespace graphic
{
	using namespace common;

	class cColor
	{
	public:
		cColor();
		cColor(const float r, const float g, const float b, const float a = 1.f);
		cColor(const BYTE r, const BYTE g, const BYTE b, const BYTE a = 255);
		cColor(const Vector3 &color);
		cColor(const Vector4 &color);
		cColor(const DWORD color);

		static const cColor BLACK;
		static const cColor WHITE;
		static const cColor GRAY;
		static const cColor GRAY0;
		static const cColor GRAY2;
		static const cColor BLUE;
		static const cColor GREEN;
		static const cColor RED;
		static const cColor YELLOW;
		static const cColor YELLOW0;
		static const cColor ORANGE;
		static const cColor KHAKI;
		static const cColor VIOLET;
		static const cColor INDIGO;
		static const cColor SKYBLUE;
		static const cColor IVORY;
		static const cColor AQUA;
		static const cColor DARKBLUE;

		virtual ~cColor();

		void SetColor(const float r, const float g, const float b, const float a=1.f);
		void SetColor(const BYTE r, const BYTE g, const BYTE b, const BYTE a=255);
		void SetColor(const Vector3 &color);
		void SetColor(const Vector4 &color);
		Vector4 GetColor() const;
		void GetColor(OUT float out[4]) const;
		DWORD GetAbgr() const;

		operator DWORD ();
		bool operator == (const cColor &color);
		bool operator != (const cColor &color);


	public:
		DWORD m_color; // ARGB
	};
}
