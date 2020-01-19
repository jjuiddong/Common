//
// 2017-08-01, jjuiddong
// Upgrade DX9 - DX11
// Material Class
//
#pragma once


namespace graphic
{
	using namespace common;

	struct sCbMaterial
	{
		XMVECTOR ambient;
		XMVECTOR diffuse;
		XMVECTOR specular;
		XMVECTOR emissive;
		float pow;
	};


	struct sMaterial;
	class cShader;
	class cRenderer;

	class cMaterial
	{
	public:
		cMaterial();
		virtual ~cMaterial();
		
		void Init(const Vector4 &ambient, 
			const Vector4 &diffuse,
			const Vector4 &specular,
			const Vector4 &emmisive=Vector4(0,0,0,1),
			const float pow=90);

		void Init(const sMaterial &mtrl);

		void InitWhite();
		void InitBlack();
		void InitGray();
		void InitGray2();
		void InitGray3();
		void InitGray4();
		void InitXFile();
		void InitRed();
		void InitBlue();
		void InitGreen();
		void InitYellow();

		Str64 DiffuseColor();
		Str64 AmbientColor();
		Str64 SpecularColor();
		Str64 EmissiveColor();
		Str64 SpecialColor(const float r, const float g, const float b);
		sCbMaterial GetMaterial();


	public:
		Vector4 m_ambient;
		Vector4 m_diffuse;
		Vector4 m_specular;
		Vector4 m_emissive;
		float m_power;
	};

}
