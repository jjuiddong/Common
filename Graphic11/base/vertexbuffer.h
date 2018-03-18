//
// 2017-07-27, jjuiddong
// Upgrade DX9 - DX11
// VertexBuffer Class
//
#pragma once


namespace graphic
{
	using namespace common;

	class cRenderer;

	class cVertexBuffer
	{
	public:
		cVertexBuffer();
		cVertexBuffer(const cVertexBuffer &rhs);
		virtual ~cVertexBuffer();

		bool Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex
			, const D3D11_USAGE usage= D3D11_USAGE_DEFAULT);
		bool Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex, void *vertices
			, const D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

		void* Lock(cRenderer &renderer, const D3D11_MAP flag = D3D11_MAP_WRITE_DISCARD);
		void Unlock(cRenderer &renderer);
		void Bind(cRenderer &renderer) const;
		void Clear();

		void RenderTriangleList(cRenderer &renderer);
		void RenderTriangleStrip(cRenderer &renderer );
		void RenderLineStrip(cRenderer &renderer );
		void RenderLineList(cRenderer &renderer);
		void RenderLineList2(cRenderer &renderer);
		void RenderPointList(cRenderer &renderer, const int count = 0);
		void RenderPointList2(cRenderer &renderer, const int count = 0);

		int GetSizeOfVertex() const;
		int GetVertexCount() const;
		void Set(cRenderer &renderer, const cVertexBuffer &rhs);

		cVertexBuffer& operator=(const cVertexBuffer &rhs);


	public:
		ID3D11Buffer *m_vtxBuff;
		int m_sizeOfVertex; // stride
		int m_vertexCount;
	};

	
	inline int cVertexBuffer::GetSizeOfVertex() const { return m_sizeOfVertex; }
	inline int cVertexBuffer::GetVertexCount() const { return m_vertexCount; }
}
