//
// 2017-10-02, jjuiddong
// Torus
//
#pragma once


namespace graphic
{
	using namespace common;

	class cTorus : public cNode
	{
	public:
		cTorus();
		virtual ~cTorus();

		bool Create(cRenderer &renderer, const float outerRadius, const float innerRadius
			, const int stack=10, const int slice = 10
			, const int vtxType = (eVertexType::POSITION)
			, const cColor &color=cColor::WHITE);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1);


	public:
		cTorusShape m_shape;
		cColor m_color;
	};

}
