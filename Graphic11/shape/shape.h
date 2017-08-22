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
			, NORMAL = 1 << 1
			, DIFFUSE = 1 << 2
			, TEXTURE = 1 << 3
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
