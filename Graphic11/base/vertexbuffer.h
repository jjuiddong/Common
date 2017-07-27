//
// 2017-07-27, jjuiddong
// Upgrade DX9 - DX11
// VertexBuffer Class
//
#pragma once


namespace graphic
{
	class cRenderer;

	class cVertexBuffer
	{
	public:
		cVertexBuffer();
		virtual ~cVertexBuffer();

		bool Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex);
		bool Create(cRenderer &renderer, const int vertexCount, const int sizeofVertex, void *vertices);
		bool CreateVMem(cRenderer &renderer, const int vertexCount, const int sizeofVertex);

		void* Lock();
		void* LockDiscard(const int idx=0, const int size=0);
		void* LockNooverwrite(const int idx=0, const int size=0);
		void Unlock();
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
		void Set(cRenderer &renderer, cVertexBuffer &rhs);


	private:
		ID3D11Buffer *m_vtxBuff;
		int m_sizeOfVertex; // stride
		int m_vertexCount;
		bool m_isManagedPool;
	};

	
	inline int cVertexBuffer::GetSizeOfVertex() const { return m_sizeOfVertex; }
	inline int cVertexBuffer::GetVertexCount() const { return m_vertexCount; }
}
