//
// 2017-05-07, jjuiddong refactoring
// Upgrade Shadow1
// Multiple Model Shadow Map
//
#pragma once


namespace graphic
{
	class cModel2;

	class cShadowMap
	{
	public:
		cShadowMap();
		virtual ~cShadowMap();

		bool Create(cRenderer &renderer, const int textureWidth, const int textureHeight);
		void Bind(cShader &shader, const string &key);
		void Begin(cRenderer &renderer);
		void End();
		bool IsLoaded() const;
		void LostDevice();
		void ResetDevice(graphic::cRenderer &renderer);

		// 디버깅용 함수.
		void RenderShadowMap(cRenderer &renderer);


	public:
		cSurface2 m_surface;
	};


	inline bool cShadowMap::IsLoaded() const { return m_surface.IsLoaded(); }
}
