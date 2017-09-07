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
			, const char *shaderName=""
			, const char *techniqueName="Unlit"
			, const bool isParallel=false
			, const bool isShadow=false
		);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;
		virtual bool RenderInstancing(cRenderer &renderer
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1
			) override;
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
		StrId m_animationName;
		eState::Enum m_state;
		cAssimpModel *m_model; // reference
	};

}
