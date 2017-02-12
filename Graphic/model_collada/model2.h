//
// 2017-02-10, jjuiddong
// collada export model
//
#pragma once

#include "node2.h"

namespace graphic 
{

	class cModel2
	{
	public:
		cModel2();
		virtual ~cModel2();

		virtual bool Create(cRenderer &renderer, const string &modelName,
			MODEL_TYPE::TYPE type = MODEL_TYPE::AUTO, const bool isLoadShader = true);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		bool Update(const float deltaSeconds);
		void Clear();


	protected:
		void UpdateBoundingBox();


	public:
		cNode2 *m_root;
		vector<Matrix44> m_tmPalette; // cNode2 id is Index
		cBoundingBox m_boundingBox;
	};

}
