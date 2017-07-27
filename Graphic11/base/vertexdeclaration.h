//
// 2017-07-27, jjuiddong
// Dx11 upgrade
// presentation Vertex Structure
//
#pragma once


namespace graphic
{

	class cVertexDeclaration
	{
	public:
		cVertexDeclaration();
		virtual ~cVertexDeclaration();

		bool Create(const sRawMesh &rawMesh );
		bool Create(const sRawMesh2 &rawMesh);
		bool Create(cRenderer &renderer, ID3DBlob *vsBlob, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements);
		int GetOffset( const BYTE usage, const BYTE usageIndex=0 ) const;
		const vector<D3D11_INPUT_ELEMENT_DESC>& GetDecl() const;
		int GetElementSize() const;
		void Bind(cRenderer &renderer);


	protected:
		void CreateDecl(
			const vector<Vector3> &vertices,
			const vector<Vector3> &normals,
			const vector<Vector3> &tex,
			const vector<Vector3> &tangent,
			const vector<Vector3> &binormal,
			const vector<sVertexWeight> &weights
			);


	public:
		ID3D11InputLayout *m_vertexLayout;
		vector<D3D11_INPUT_ELEMENT_DESC> m_decl;
		int m_elementSize;
	};


	inline const vector<D3D11_INPUT_ELEMENT_DESC>& cVertexDeclaration::GetDecl() const { return m_decl; }
	inline int cVertexDeclaration::GetElementSize() const { return m_elementSize; }
}
