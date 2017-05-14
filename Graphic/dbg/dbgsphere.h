//
// 2017-05-13
//	 Debug Sphere
//
#pragma once

#include "../model/node.h"

namespace graphic
{

	class cDbgSphere
	{
	public:
		cDbgSphere();
		cDbgSphere(cRenderer &renderer, const float radius, const int stacks, const int slices);
		virtual ~cDbgSphere();

		void Create(cRenderer &renderer, const float radius, const int stacks, const int slices);
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		float m_radius;
		int m_numSlices;
		int m_numStacks;
		Matrix44 m_tm;
	};

}
