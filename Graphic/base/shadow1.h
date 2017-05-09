//
// 2017-05-07, jjuiddong refactoring
// 투영 그림자 클래스
// 모델 하나의 그림자 텍스쳐를 만든다.
// 하나 이상의 모델의 그림자를 표현 할 때는 이 클래스를 사용하지 않는게 좋다.
// 깊이 값은 저장하지 않는, true/false 그림자 텍스쳐를 이용한다.
//
#pragma once


namespace graphic
{
	class cModel2;

	class cShadow1
	{
	public:
		cShadow1();
		virtual ~cShadow1();

		bool Create(cRenderer &renderer, const int textureWidth, const int textureHeight);
		void UpdateShadow(cRenderer &renderer, cNode &node);
		void UpdateShadow(cRenderer &renderer, cModel2 &model);
		void Bind(cShader &shader, const string &key);
		void Begin(cRenderer &renderer);
		void End();
		IDirect3DTexture9* GetTexture();
		const IDirect3DTexture9* GetTexture() const;
		bool IsLoaded() const;
		void LostDevice();
		void ResetDevice(graphic::cRenderer &renderer);

		// 디버깅용 함수.
		void RenderShadowMap(cRenderer &renderer);


	public:
		cSurface2 m_surface;
	};


	inline IDirect3DTexture9* cShadow1::GetTexture() { return m_surface.GetTexture(); }
	inline const IDirect3DTexture9* cShadow1::GetTexture() const { return m_surface.GetTexture(); }
	inline bool cShadow1::IsLoaded() const { return m_surface.IsLoaded(); }
}
