//
// Water Effect
// Frank Luna, Pond Water 예제를 참조해서 만들었다.
// http://blog.naver.com/swoosungi/90086817771
//
// 2017-09-11
//	- DX11 Upgrade
//
#pragma once


namespace graphic
{
	using namespace common;

	struct sCbWater
	{
		XMMATRIX gWorldInv;
		XMFLOAT2 gWaveMapOffset0;
		XMFLOAT2 gWaveMapOffset1;
		XMFLOAT2 gRippleScale;
		float gRefractBias;
		float gRefractPower;
	};


	class cWater : public cNode
	{
	public:
		cWater();
		virtual ~cWater();

		bool Create(cRenderer &renderer);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;
		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;

		void BeginRefractScene(cRenderer &renderer);
		void EndRefractScene(cRenderer &renderer);
		void BeginReflectScene(cRenderer &renderer);
		void EndReflectScene(cRenderer &renderer);
		void SetRenderReflectMap(const bool enable);
		bool IstRenderReflectMap() const;
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		Matrix44 GetReflectionMatrix();
		Plane GetWorldPlane();


	public:
		struct sInitInfo
		{
			cLight dirLight;
			cMaterial mtrl;
			int vertRows;
			int vertCols;
			float cellSize;
			float dx;
			float dz;
			float uvFactor;
			float yOffset;
			StrPath waveMapFilename0;
			StrPath waveMapFilename1;
			Vector2 waveMapVelocity0;
			Vector2 waveMapVelocity1;
			float texScale;
			float refractBias;
			float refractPower;
			Vector2 rippleScale;
			Matrix44 toWorld;
		};

		sInitInfo m_initInfo;
		cRenderTarget m_reflectMap;
		cRenderTarget m_refractMap;
		cGrid m_grid;
		cShader11 *m_shader; // reference
		cTexture *m_waveMap0; // reference
		cTexture *m_waveMap1; // reference
		cConstantBuffer<sCbWater> m_cbWater;

		Vector2 m_waveMapOffset0;
		Vector2 m_waveMapOffset1;
		bool m_isRenderSurface;
	};

	
	inline void cWater::SetRenderReflectMap(const bool enable) { m_isRenderSurface = enable; }
	inline bool cWater::IstRenderReflectMap() const { return m_isRenderSurface; }
}
