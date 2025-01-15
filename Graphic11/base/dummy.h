//
// 2024-12-14, jjuiddong
// DX11 Dummy Class
//
#pragma once


namespace graphic
{
	using namespace common;

	class cRenderer;
	class cDummy : public cNode
	{
	public:
		cDummy();
		cNode* Clone(cRenderer& renderer) const;
	};	

}
