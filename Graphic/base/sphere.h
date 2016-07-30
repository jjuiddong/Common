//
// 2016-05-22, jjuiddong
// 구 를 출력하는 클래스.
// 
// 2016-05-22
//		- cNode 클래스 상속받아서 일반화 시킴
//
#pragma once

#include "../model/node.h"

namespace graphic
{

	class cSphere : public cNode
	{
	public:
		cSphere();
		cSphere(cRenderer &renderer, const float radius, const int stacks, const int slices);
		virtual ~cSphere();

		void Create(cRenderer &renderer, const float radius, const int stacks, const int slices);
		virtual void Render(cRenderer &renderer, const Matrix44 &tm) override;
		void RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		float m_radius;
		int m_numSlices;
		int m_numStacks;
		cMaterial m_mtrl;
	};	

}
