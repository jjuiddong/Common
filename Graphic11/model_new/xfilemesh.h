//
// 2017-01-24, jjuiddong
// Direct3D9 XFile Loading class
//
#pragma once


namespace graphic
{

	class cXFileMesh
	{
	public:
		cXFileMesh();
		virtual ~cXFileMesh();

		bool Create(cRenderer &renderer, const StrPath &fileName, const bool isShadow=false, const bool isOptimize=false);
		bool Update(const float deltaSeconds);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm = Matrix44::Identity);
		void RenderShadow(cRenderer &renderer, cShader &shader);
		bool IsLoad();
		void Clear();


	protected:
		HRESULT CreateMaterials(const StrPath &filePath, cRenderer &renderer, D3DXMATERIAL* d3dxMtrls, const DWORD dwNumMaterials);
		void InitBoundingBox();
		

	public:
		ID3DXMesh *m_mesh;
		int m_verticesCount;
		int m_facesCount;
		int m_materialsCount;
		int m_stride;
		cBoundingBox m_boundingBox; // Local Space
		cMaterial* m_materials;
		cTexture** m_textures; // reference 
		cShadowVolume m_shadow;
	};
}
