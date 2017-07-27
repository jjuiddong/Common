//
// 2017-05-07, jjuiddong refactoring
// Upgrade Shadow1
// Multiple Model Shadow Map
//
#pragma once


namespace graphic
{
	class cModel2;

	class cShadow2
	{
	public:
		cShadow2();
		virtual ~cShadow2();

		bool Create(cRenderer &renderer, const int textureWidth, const int textureHeight);
		void Bind(cShader &shader, const Str32 &key);
		void Begin(cRenderer &renderer);
		void End(cRenderer &renderer);
		bool IsLoaded() const;
		void LostDevice();
		void ResetDevice(graphic::cRenderer &renderer);

		// 디버깅용 함수.
		void RenderShadowMap(cRenderer &renderer);


	public:
		cSurface2 m_surface;
	};


	inline bool cShadow2::IsLoaded() const { return m_surface.IsLoaded(); }
}
