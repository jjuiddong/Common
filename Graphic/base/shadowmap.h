//
// 2017-05-09, jjuiddong refactoring
// Shadow Map
// Direct3D9 ShadowMap Sample Reference
//
#pragma once


namespace graphic
{

	class cShadowMap
	{
	public:
		cShadowMap();
		virtual ~cShadowMap();

		bool Create(cRenderer &renderer, const int textureWidth, const int textureHeight);
		void Bind(cShader &shader, const string &key);
		void Begin(cRenderer &renderer);
		void End(cRenderer &renderer);
		bool IsLoaded() const;
		void LostDevice();
		void ResetDevice(graphic::cRenderer &renderer);

		// 디버깅용 함수.
		void RenderShadowMap(cRenderer &renderer, const int index = 1);


	public:
		cSurface3 m_surface;
	};

}
