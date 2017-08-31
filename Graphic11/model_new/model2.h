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

		bool Create( cRenderer &renderer 
			, const int id 
			, const StrPath &fileName
			, const StrPath &shaderName=""
			, const Str32 &techniqueName="Unlit"
			, const bool isParallel=false
			, const bool isShadow=false
		);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags=1);
		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual void LostDevice() {}
		virtual void ResetDevice(cRenderer &renderer) {}
		virtual void Clear();
		void SetAnimation(const Str64 &animationName, const bool isMerge = false);
		bool IsLoadFinish();


	protected:
		void InitModel(cRenderer &renderer);
		bool CheckLoadProcess(cRenderer &renderer);


	public:
		struct eState { enum Enum { 
				LOAD_PARALLEL
				, LOAD_SINGLE
				, LOAD_MESH_FINISH
				, LOAD_PARALLEL_SHADOW
				, LOAD_SINGLE_SHADOW
				, LOAD_FINISH
				, NORMAL 
			};
		};

		StrPath m_fileName;
		StrPath m_shaderName;
		Str32 m_techniqueName;
		StrId m_animationName;
		eState::Enum m_state;
		cAssimpModel *m_model; // reference
	};

}
