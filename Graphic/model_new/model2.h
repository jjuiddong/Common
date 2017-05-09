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
	class cShadowMap;

	class cModel2 : public iShaderRenderer
							, public iShadowRenderer
	{
	public:
		cModel2();
		virtual ~cModel2();

		bool Create(cRenderer &renderer, const cFilePath &fileName
			, const cFilePath &shaderName=""
			, const string &techniqueName=""
			, const bool isParallel=false
			, const bool isShadow=false
		);

		bool Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		virtual void RenderShader(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;
		virtual void RenderShadow(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;
		virtual void SetShader(cShader *shader) override;
		virtual void SetShadowShader(cShader *shader) override;
		bool Update(cRenderer &renderer, const float deltaSeconds);
		void SetAnimation(const string &animationName, const bool isMerge = false);
		bool IsLoadFinish();
		void Clear();


	protected:
		void InitModel(cRenderer &renderer);
		bool CheckLoadProcess(cRenderer &renderer);


	public:
		struct eState { enum Enum { 
				LOAD_PARALLEL_COLLADA
				, LOAD_PARALLEL_XFILE
				, LOAD_SINGLE_COLLADA
				, LOAD_SINGLE_XFILE
				, LOAD_MESH_FINISH
				, LOAD_PARALLEL_COLLADA_SHADOW
				, LOAD_PARALLEL_XFILE_SHADOW
				, LOAD_SINGLE_COLLADA_SHADOW
				, LOAD_SINGLE_XFILE_SHADOW
				, LOAD_FINISH
				, NORMAL 
			};
		};

		eState::Enum m_state;
		cFilePath m_fileName;
		bool m_isShadow; // shadow volume
		bool m_isShadow2; // shadow map
		cColladaModel *m_colladaModel;
		cXFileMesh *m_xModel;
		string m_shaderName;
		string m_techniqueName;
		Matrix44 m_tm;
		string m_animationName;
		cBoundingBox m_boundingBox;
		cShadowVolume *m_shadow; // reference
		cShadow2 *m_shadowMap; // reference
	};

}
