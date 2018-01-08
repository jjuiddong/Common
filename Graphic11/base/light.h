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
		XMVECTOR specIntensity; // x = specular intensity exp, y = specular intensity
	};

	class cCamera;

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
			//const Vector4 &ambient = Vector4(1,1,1,1),
			const Vector4 &ambient = Vector4(0.3f, 0.3f, 0.3f, 1),
			const Vector4 &diffuse = Vector4(0.9f, 0.9f, 0.9f,1),
			const Vector4 &specular = Vector4(0.2f, 0.2f, 0.2f, 0.2f),
			const Vector3 &direction = Vector3(0,-1,0));

		void Set(const cCamera &camera);
		void SetDirection( const Vector3 &direction );
		void SetPosition( const Vector3 &pos );
		const Vector3& GetDirection() const;
		const Vector3& GetPosition() const;
		void Bind(cRenderer &renderer);
		sCbLight GetLight();


	public:
		TYPE m_type;
		Vector4 m_ambient;
		Vector4 m_diffuse;
		Vector4 m_specular;
		Vector3 m_direction;
		Vector3 m_pos;
		float m_specExp;
		float m_specIntensity;
	};


	inline const Vector3& cLight::GetDirection() const { return m_direction; }
	inline const Vector3& cLight::GetPosition() const { return m_pos; }
}
