//
// 2017-05-06, jjuiddong
// grid2 Simple Version
// format = point + normal + texture
//
#pragma once


namespace graphic
{

	class cGrid3 : public graphic::iShaderRenderer
	{
	public:
		cGrid3();
		virtual ~cGrid3();

		void Create(cRenderer &renderer, const int rowCellCount, const int colCellCount, const float cellSize
			, const float textureUVFactor = 8.f, const float offsetY = 0.f
			, const Vector2 &uv0=Vector2(0,0), const Vector2 &uv1 = Vector2(1, 1));
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity, const int stage = 0);
		virtual void RenderShader(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;
		void RenderLinelist(cRenderer &renderer);
		void CalculateNormals();
		bool Pick(const Vector3 &orig, const Vector3 &dir, Vector3 &out);
		void SetTextureUVFactor(const float textureUVFactor);
		void Clear();


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		cTexture *m_tex;
		cMaterial m_mtrl;
		int m_rowCellCount;
		int m_colCellCount;
		float m_cellSize;
		float m_textureUVFactor;
	};
	
}
