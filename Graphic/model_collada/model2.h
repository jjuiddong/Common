//
// 2017-02-16, jjuiddong
// collada model
// collada architecture model
//
#pragma once


namespace graphic
{

	class cModel2
	{
	public:
		cModel2();
		virtual ~cModel2();
		bool Create(cRenderer &renderer, const string &fileName);
		bool Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		bool Update(const float deltaSeconds);
		void SetAnimation(const string &animationName, const bool isMerge=false);
		void SetShader(cShader *shader);
		void Clear();


	protected:
		void UpdateBoundingBox();


	public:
		vector<cMesh2*> m_meshes;
		cSkeleton m_skeleton;
		cAnimation m_animation;
		cBoundingBox m_boundingBox;
		string m_storedAnimationName;
		cShader *m_shader; // reference
	};

}
