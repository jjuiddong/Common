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

		bool Create(cRenderer &renderer, const string &fileName);
		bool Update(const float deltaSeconds);
		void Render(cRenderer &renderer);
		void SetShader(cShader *shader);
		void Clear();


	protected:
		HRESULT CreateMaterials(const string &filePath, cRenderer &renderer, D3DXMATERIAL* d3dxMtrls, const DWORD dwNumMaterials);
		void InitBoundingBox();
		

	public:
		ID3DXMesh *m_mesh;
		Matrix44 m_tm;
		int m_verticesCount;
		int m_facesCount;
		int m_materialsCount;
		int m_stride;
		cBoundingBox m_boundingBox;
		cShader *m_shader; // reference
		cMaterial* m_materials;
		cTexture** m_textures; // reference texture
	};


	inline void cXFileMesh::SetShader(cShader *shader) { m_shader = shader;  }
}
