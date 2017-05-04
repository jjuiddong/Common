//
// 2017-02-16, jjuiddong
// collada model
// collada architecture model
//
#pragma once


namespace graphic
{

	class cColladaModel
	{
	public:
		cColladaModel();
		virtual ~cColladaModel();
		bool Create(cRenderer &renderer, const string &fileName);
		bool Render(cRenderer &renderer, cShader &shader, const Matrix44 &tm = Matrix44::Identity);
		bool Update(const float deltaSeconds);
		void SetAnimation(const string &animationName, const bool isMerge=false);
		void Clear();


	protected:
		void UpdateBoundingBox();


	public:
		bool m_isSkinning;
		vector<cMesh2*> m_meshes;
		cSkeleton m_skeleton;
		cAnimation m_animation;
		cBoundingBox m_boundingBox;
		string m_storedAnimationName;
	};

}
