//
// 2017-08-04, jjuiddong
// Primitive Shape Class
//
#pragma once


namespace graphic
{

	struct eVertexType {
		enum Enum {
			POSITION = 1
			, POSITION_RHW	= 1 << 1
			, NORMAL		= 1 << 2
			, COLOR			= 1 << 3
			, TEXTURE0		= 1 << 4
			, TEXTURE1		= 1 << 5
			, TANGENT		= 1 << 6
			, BINORMAL		= 1 << 7
			, BLENDINDICES	= 1 << 8
			, BLENDWEIGHT	= 1 << 9
		};
	};


	interface cShape
	{
	public:
		cShape() : m_vtxType(0) {}
		virtual ~cShape() {}

		virtual void Render(cRenderer &renderer) = 0;

		int m_vtxType;
	};

}
