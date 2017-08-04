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
		cDbgSphere(cRenderer &renderer, const float radius, const int stacks, const int slices, const cColor &color = cColor::BLACK);
		virtual ~cDbgSphere();

		void Create(cRenderer &renderer, const float radius, const int stacks, const int slices, const cColor &color = cColor::BLACK);
		void Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);
		void SetPos(const Vector3 &pos);
		XMMATRIX GetTransform() const;


	public:
		cSphereShape m_shape;
		cBoundingSphere m_bsphere;
	};

}
