//
// 2017-07-05, jjuiddong
// Quad 2D
//
#pragma once


namespace graphic
{

	class cQuad2D
	{
	public:
		cQuad2D();
		virtual ~cQuad2D();

		bool Create(cRenderer &renderer
			, const float x, const float y
			, const float width, const float height
			, const StrPath &textureFileName = ""
			, const bool isSizePow2 = true);

		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void SetPosition(const float x, const float y
			, const float width, const float height
		);


	public:
		cVertexBuffer m_vtxBuff;
		cMaterial m_material;
		cTexture *m_texture; // reference
	};

}
