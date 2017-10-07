//
// 2017-02-16, jjuiddong
// assimpl loader model
// assimp architecture model
//
#pragma once


namespace graphic
{

	class cAssimpModel
	{
	public:
		cAssimpModel();
		virtual ~cAssimpModel();
		bool Create(cRenderer &renderer, const StrPath &fileName);
		bool Render(cRenderer &renderer
			, const char *techniqueName
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);
		bool RenderInstancing(cRenderer &renderer
			, const char *techniqueName
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);
		bool Update(const float deltaSeconds);
		void SetAnimation(const Str64 &animationName, const bool isMerge=false);
		int GetVertexType();
		void Clear();


	protected:
		void UpdateBoundingBox();
		bool RenderNode(cRenderer &renderer
			, const char *techniqueName
			, const sRawNode &node
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);


	public:
		bool m_isSkinning;
		vector<cMesh2*> m_meshes;
		vector<sRawNode> m_nodes;
		cSkeleton m_skeleton;
		cAnimation m_animation;
		cBoundingBox m_boundingBox;
		StrId m_storedAnimationName;
	};

}
