//
// 2017-09-28, jjuiddong
// Temporal Buffer
//
#pragma once


namespace graphic
{
	class cTemporalBuffer
	{
	public:
		cTemporalBuffer();
		virtual ~cTemporalBuffer();

		bool Create(cRenderer &renderer, const int bytesSize, const int stride);
		bool Create(cRenderer &renderer, const cVertexBuffer &vtxBuff);
		bool Create(cRenderer &renderer, const cIndexBuffer &idxBuff);
		bool CopyFrom(cRenderer &renderer, const cVertexBuffer &vtxBuff);
		bool CopyFrom(cRenderer &renderer, const cIndexBuffer &idxBuff);

		void* Lock(cRenderer &renderer, const D3D11_MAP flag = D3D11_MAP_READ);
		void Unlock(cRenderer &renderer);


	public:
		ID3D11Buffer *m_buff;
		int m_stride; // byte size of element
		int m_elementCount;
	};
}
