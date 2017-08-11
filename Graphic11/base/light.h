//
// 2017-07-31, jjuiddong
// Lighting Class
// Upgrade DX9 - DX11
//
#pragma once


namespace graphic
{

	struct sCbLight
	{
		XMVECTOR ambient;
		XMVECTOR diffuse;
		XMVECTOR specular;
		XMVECTOR direction;
		XMVECTOR posW;
	};


	class cLight
	{
	public:
		cLight();
		virtual ~cLight();

		enum TYPE 
		{
			LIGHT_POINT = 1,
			LIGHT_SPOT = 2,
			LIGHT_DIRECTIONAL = 3,
		};

		void Init(TYPE type, 
			const Vector4 &ambient = Vector4(0.7f,0.7f,0.7f,1),
			const Vector4 &diffuse = Vector4(1,1,1,1),
			const Vector4 &specular = Vector4(1,1,1,1),
			const Vector3 &direction = Vector3(0,-1,0));

		void SetDirection( const Vector3 &direction );
		void SetPosition( const Vector3 &pos );
		//const Vector3& GetDirection() const;
		//const Vector3& GetPosition() const;

		void Bind(cRenderer &renderer, int lightIndex=0);
		//void Bind(cShader &shader) const;

		void GetShadowMatrix( const Vector3 &modelPos, 
			OUT Vector3 &lightPos, OUT Matrix44 &view, OUT Matrix44 &proj, 
			OUT Matrix44 &tt );
		void GetShadowMatrix(const Vector3 &lightPos, OUT Matrix44 &view
			, OUT Matrix44 &proj, OUT Matrix44 &tt);
		void GetShadowMatrix(OUT Matrix44 &view, OUT Matrix44 &proj, OUT Matrix44 &tt);

		sCbLight GetLight();


	public:
		TYPE m_type;
		Vector4 m_ambient;
		Vector4 m_diffuse;
		Vector4 m_specular;
		Vector3 m_direction;
		Vector3 m_pos;

		//XMFLOAT4 m_ambient;
		//XMFLOAT4 m_diffuse;
		//XMFLOAT4 m_specular;
		//XMFLOAT3 m_direction;
		//XMFLOAT3 m_pos;
	};


	//inline const Vector3& cLight::GetDirection() const { return *(Vector3*)&m_light.Direction; }
	//inline const Vector3& cLight::GetPosition() const { return *(Vector3*)&m_light.Position; }
}
