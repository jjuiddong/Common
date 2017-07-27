#pragma once


namespace graphic
{

	class cSkinnedMesh : public cMesh
	{
	public:
		cSkinnedMesh(cRenderer &renderer, const int id, vector<Matrix44> *palette, const sRawMesh &raw);
		virtual ~cSkinnedMesh();

		virtual void Render(cRenderer &renderer, const Matrix44 &parentTm) override;

		void SetPalette(vector<Matrix44> *palette);


	protected:
		virtual void RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &parentTm);
		virtual void RenderShadow(cRenderer &renderer, cShader &shader, const Matrix44 &parentTm);

		void ApplyPalette(cRenderer &renderer);
		void ApplyPaletteShader(cShader &shader);


	private:
		const sRawMesh &m_rawMesh;
		vector<Matrix44> *m_palette; // reference
		cMeshBuffer *m_skinnMeshBuffer;
	};


	inline void cSkinnedMesh::SetPalette(vector<Matrix44> *palette) { m_palette = palette; }
}
