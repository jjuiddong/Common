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
	using namespace common;
	class cAssimpModel;
	class cShadowMap;

	class cModel : public cNode
	{
	public:
		cModel();
		virtual ~cModel();

		bool Create( cRenderer &renderer 
			, const int id 
			, const StrPath &fileName
			, const bool isParallel=false
			, const bool isShadow=true
		);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		virtual bool RenderInstancing(cRenderer &renderer
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1
			) override;

		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		void SetAnimation(const Str64 &animationName, const bool isMerge = false);
		void SetAnimation(const int animationIndex, const bool isMerge = false);
		bool IsLoadFinish();
		virtual void SetShader(cShader11 *shader) override;
		virtual cNode* Clone(cRenderer &renderer) const override;
		virtual void Clear();


	protected:
		virtual void InitModel(cRenderer &renderer);
		bool CheckLoadProcess(cRenderer &renderer);
		bool ReadModelInformation(const StrPath &modelInfoFileName);


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

		bool m_isLoad; // is Model Loaded?
		StrPath m_fileName;
		StrId m_animationName;
		eState::Enum m_state;
		cAssimpModel *m_model; // reference
		cSkeleton m_skeleton;
		cAnimation m_animation;
		Transform m_localTm; // tricky code (solution: change mesh position, orientation)
		vector<int> m_meshInVisible; // mesh invisible index array, tricky code, need optimize,
		float m_aniIncT;
		float m_animationSpeed;
	};

}
