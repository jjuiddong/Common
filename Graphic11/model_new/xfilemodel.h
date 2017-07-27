//
// 2017-05-01, jjuiddong
// xfilemesh model
//
#pragma once


namespace graphic
{
	class cXFileMesh;

	class cXFileModel : public iShaderRenderer
	{
	public:
		cXFileModel() : m_mesh(NULL) {}
		virtual ~cXFileModel() {}
		bool Create(cRenderer &renderer, const string &fileName, const bool isShadow = false);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		virtual void RenderShader(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;


	public:
		cXFileMesh *m_mesh;
		Matrix44 m_tm;
	};

}
