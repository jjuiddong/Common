//
// 2017-09-27, jjuiddong
// Cascaded ShadowMap for Terrain
//
#pragma once


namespace graphic
{

	class cCascadedShadowMap
	{
	public:
		cCascadedShadowMap();
		virtual ~cCascadedShadowMap();

		bool Create(cRenderer &renderer
			, const Vector2 &shadowMapSize = Vector2(1024,1024)
			, const float z0 = 30.f
			, const float z1 = 100.f
			, const float z2 = 300.f
		);
		bool UpdateParameter(cRenderer &renderer, const cCamera &camera);
		bool Bind(cRenderer &renderer);
		bool Begin(cRenderer &renderer, const int shadowMapIndex);
		bool End(cRenderer &renderer, const int shadowMapIndex);
		void DebugRender(cRenderer &renderer);


	public:
		enum { SHADOWMAP_COUNT = 3 };
		Vector2 m_shadowMapSize;
		float m_splitZ[3];
		cCamera m_lightCams[SHADOWMAP_COUNT];
		cDbgFrustum m_frustums[SHADOWMAP_COUNT];
		cRenderTarget m_shadowMaps[SHADOWMAP_COUNT];
		cQuad2D m_shadowMapQuads[SHADOWMAP_COUNT];
		cDbgFrustum m_dbgLightFrustums[SHADOWMAP_COUNT];
	};

}
