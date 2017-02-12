//
// 2017-02-12, jjuiddong
// collada model node
//
#pragma once

#include "mesh2.h"

namespace graphic
{

	class cNode2
	{
	public:
		cNode2();
		virtual ~cNode2();

		bool Create(cRenderer &renderer, const sRawNode &rawNode, vector<Matrix44> *tmPalette);
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		bool Update(const float deltaSeconds, const Matrix44 &tm = Matrix44::Identity);
		void Clear();


	public:
		int m_id;
		bool m_isVisible;
		string m_name;
		Matrix44 m_localTm;
		Matrix44 m_aniTm;
		Matrix44 m_Tm;
		Vector3 m_scale;
		vector<cMesh2*> m_meshes;
		vector<cNode2*> m_children;
		vector<Matrix44> *m_tmPalette; // reference
	};

}
