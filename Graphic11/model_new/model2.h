//
// 2017-05-01, jjuiddong
// integrate model class
//	- assimp load model
//
// 2017-08-10
//	- Upgrade DX11
//
#pragma once


namespace graphic
{
	class cAssimpModel;
	class cShadowMap;

	class cModel2 : public cNode2
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

		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags=1);
		//virtual void SetShader(cShader *shader) override;
		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual void LostDevice() {}
		virtual void ResetDevice(cRenderer &renderer) {}
		virtual void Clear();
		void SetAnimation(const Str64 &animationName, const bool isMerge = false);
		bool IsLoadFinish();
		void UpdateShader(cRenderer &renderer);


	protected:
		void InitModel(cRenderer &renderer);
		bool CheckLoadProcess(cRenderer &renderer);


	public:
		struct eState { enum Enum { 
				LOAD_PARALLEL_ASSIMP
				, LOAD_SINGLE_ASSIMP
				, LOAD_MESH_FINISH
				, LOAD_PARALLEL_ASSIMP_SHADOW
				, LOAD_SINGLE_ASSIMP_SHADOW
				, LOAD_FINISH
				, NORMAL 
			};
		};

		StrPath m_fileName;
		StrPath m_shaderName;
		Str32 m_techniqueName;
		StrId m_animationName;
		eState::Enum m_state;
		//D3DCULL m_cullType; // default : CCW
		cAssimpModel *m_assimpModel; // reference
		//cShadow2 *m_shadowMap; // reference
	};

}
