//
// 2017-05-15, jjuiddong
// 절두체 디버깅 클래스.
//
#pragma once


namespace graphic
{
	class cRenderer;

	class cDbgFrustum : public cFrustum
									, public iShaderRenderer
	{
	public:
		cDbgFrustum();
		virtual ~cDbgFrustum();

		bool Create(cRenderer &renderer, const Matrix44 &matViewProj);
		bool Create(cRenderer &renderer, const Vector3 &_min, const Vector3 &_max);
		void SetFrustum(cRenderer &renderer, const Matrix44 &matViewProj);
		void Render(cRenderer &renderer);
		virtual void RenderShader(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;


	public:
		cDbgBox m_box;
	};

}
