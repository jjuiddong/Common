//
// 2017-07-05, jjuiddong
// Quad 2D
//
// 2017-09-06
//	- Upgrade DX11
//
#pragma once


namespace graphic
{

	class cQuad2D : public cNode2
	{
	public:
		cQuad2D();
		virtual ~cQuad2D();

		bool Create(cRenderer &renderer, const float x, const float y, const float width, const float height
			, const char *textureFileName = "" );

		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);

		void SetPosition(const float x, const float y
			, const float width, const float height );


	public:
		cQuadShape m_shape;
		cTexture *m_texture; // reference
	};

}
