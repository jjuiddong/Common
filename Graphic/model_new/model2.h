//
// 2017-05-01, jjuiddong
// integrate model class
//	- collada, xfile
//
#pragma once


namespace graphic
{
	class cColladaModel;
	class cXFileMesh;

	class cModel2 : public iShaderRenderer
	{
	public:
		cModel2();
		virtual ~cModel2();

		bool Create(cRenderer &renderer, const cFilePath &fileName, const cFilePath &shaderName="", const string &techniqueName="");
		bool Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		virtual void RenderShader(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;
		bool Update(const float deltaSeconds);
		void SetAnimation(const string &animationName, const bool isMerge = false);
		void Clear();


	public:
		cFilePath m_fileName;
		cColladaModel *m_colladaModel;
		cXFileMesh *m_xModel;

		Matrix44 m_tm;
		string m_animationName;
		cBoundingBox m_boundingBox;
		cShadowVolume m_shadow;
	};

}
