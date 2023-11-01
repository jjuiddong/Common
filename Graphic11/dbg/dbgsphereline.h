//
// 2023-11-01, jjuiddong
// Debug Sphere Line
//	- billboard DbgCircleLine
//
#pragma once


namespace graphic
{
	using namespace common;
	class cRenderer;

	class cDbgSphereLine : public cDbgCircleLine
	{
	public:
		cDbgSphereLine();
		cDbgSphereLine(cRenderer &renderer, const float radius, const int slice
			, const cColor &color=cColor::BLACK);

		void Render(cRenderer &renderer, const XMMATRIX &tm=XMIdentity);
		void RenderInstancing(cRenderer &renderer
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1
		);

	};

}
