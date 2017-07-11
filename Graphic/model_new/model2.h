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

	class cModel2 : public cNode2
				, public iShaderRenderer
				, public iShadowRenderer
	{
	public:
		cModel2();
		virtual ~cModel2();

		bool Create(cRenderer &renderer 
			, const int id 
			, const StrPath &fileName
			, const StrPath &shaderName=""
			, const Str32 &techniqueName=""
			, const bool isParallel=false
			, const bool isShadow=false
		);

		virtual bool Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity, const int flags=1) override;
		virtual void RenderShader(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;
		virtual void RenderShadow(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;
		virtual void SetShader(cShader *shader) override;
		virtual void SetShadowShader(cShader *shader) override;
		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual void LostDevice() {}
		virtual void ResetDevice(cRenderer &renderer) {}
		virtual void Clear();
		void SetAnimation(const Str64 &animationName, const bool isMerge = false);
		bool IsLoadFinish();
		void CalcBoundingSphere();
		void UpdateShader(cRenderer &renderer);


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

		StrPath m_fileName;
		StrPath m_shaderName;
		Str32 m_techniqueName;
		StrId m_animationName;
		eState::Enum m_state;
		bool m_isShadowEnable;
		bool m_isShadow; // shadow volume
		bool m_isShadow2; // shadow map
		bool m_isAlphablend;
		D3DCULL m_cullType; // default : CCW
		cColladaModel *m_colladaModel; // reference
		cXFileMesh *m_xModel; // reference
		cBoundingBox m_boundingBox; // Local Space
		cBoundingSphere m_boundingSphere;
		cShadowVolume *m_shadow; // reference
		cShadow2 *m_shadowMap; // reference
	};

}
