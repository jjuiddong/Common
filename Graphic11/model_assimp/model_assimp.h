//
// 2017-02-16, jjuiddong
// assimpl loader model
// assimp architecture model
//
#pragma once


namespace graphic
{
	using namespace common;

	class cAssimpModel
	{
	public:
		cAssimpModel();
		virtual ~cAssimpModel();
		bool Create(cRenderer &renderer, const StrPath &fileName);
		bool Render(cRenderer &renderer
			, const char *techniqueName
			, cSkeleton *skeleton
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);

		bool RenderInstancing(cRenderer &renderer
			, const char *techniqueName
			, cSkeleton *skeleton
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);

		bool RenderTessellation(cRenderer &renderer
			, const char *techniqueName
			, const int controlPointCount
			, cSkeleton *skeleton
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);

		bool Update(const float deltaSeconds);
		int GetVertexType();
		void Clear();


	protected:
		void UpdateBoundingBox();
		bool RenderNode(cRenderer &renderer
			, const char *techniqueName
			, cSkeleton *skeleton
			, const sRawNode &node
			, const XMMATRIX &parentTm = XMIdentity
			, const XMMATRIX &transformTm = XMIdentity
			, const int flags = 1);

		bool RenderNode_Tessellation(cRenderer &renderer
			, const char *techniqueName
			, const int controlPointCount
			, cSkeleton *skeleton
			, const sRawNode &node
			, const XMMATRIX &parentTm = XMIdentity
			, const XMMATRIX &transformTm = XMIdentity
			, const int flags = 1);

		bool HasAlphaBlend();


	public:
		bool m_isSkinning;
		vector<cMesh*> m_meshes;
		vector<sRawNode> m_nodes;
		cSkeleton m_skeleton;		// 원본 스켈레톤 데이타, cModel에서 복사해서 사용한다.
		cAnimation m_animation;		// 원본 애니메이션 데이타, cModel에서 복사해서 사용한다.
		cBoundingBox m_boundingBox;
		StrId m_storedAnimationName;
	};

}
