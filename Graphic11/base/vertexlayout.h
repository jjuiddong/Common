//
// 2017-07-27, jjuiddong
// Dx11 upgrade
// presentation Vertex Structure
//
#pragma once


namespace graphic
{

	class cVertexLayout
	{
	public:
		cVertexLayout();
		virtual ~cVertexLayout();

		bool Create(const sRawMesh2 &rawMesh);
		bool Create(cRenderer &renderer, ID3DBlob *vsBlob, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements);
		bool Create(cRenderer &renderer, const BYTE *pIAInputSignature, const SIZE_T IAInputSignatureSize
			, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements);
		bool Create(const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements);
		bool Create(const vector<D3D11_INPUT_ELEMENT_DESC> &layout);
		int GetOffset(const char *semanticName) const;
		const vector<D3D11_INPUT_ELEMENT_DESC>& GetLayout() const;
		int GetVertexSize() const;
		void Bind(cRenderer &renderer);
		void Clear();


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
		vector<D3D11_INPUT_ELEMENT_DESC> m_elements;
		int m_elementSize;
		int m_vertexType; // compose eVertexType
	};


	inline const vector<D3D11_INPUT_ELEMENT_DESC>& cVertexLayout::GetLayout() const { return m_elements; }
	inline int cVertexLayout::GetVertexSize() const { return m_elementSize; }
}
