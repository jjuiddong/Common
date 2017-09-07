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
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);
		bool RenderInstancing(cRenderer &renderer
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);
		bool Update(const float deltaSeconds);
		void SetAnimation(const Str64 &animationName, const bool isMerge=false);
		void Clear();


	protected:
		void UpdateBoundingBox();


	public:
		bool m_isSkinning;
		vector<cMesh2*> m_meshes;
		cSkeleton m_skeleton;
		cAnimation m_animation;
		cBoundingBox m_boundingBox;
		StrId m_storedAnimationName;
	};

}
