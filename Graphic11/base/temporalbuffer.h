//
// 2017-09-28, jjuiddong
// Temporal Buffer
//
#pragma once


namespace graphic
{
	using namespace common;

	class cTexture;
	class cTemporalBuffer
	{
	public:
		cTemporalBuffer();
		virtual ~cTemporalBuffer();

		bool Create(cRenderer &renderer, const int bytesSize, const int stride);
		bool Create(cRenderer &renderer, const cVertexBuffer &vtxBuff);
		bool Create(cRenderer &renderer, const cIndexBuffer &idxBuff);
		bool Create(cRenderer &renderer, const cTexture &texture);
		bool CopyFrom(cRenderer &renderer, const cVertexBuffer &vtxBuff);
		bool CopyFrom(cRenderer &renderer, const cIndexBuffer &idxBuff);
		bool CopyFrom(cRenderer &renderer, const cTexture &texture);
		bool CopyFrom(cRenderer &renderer, ID3D11Texture2D *texture);

		void* Lock(cRenderer &renderer, const D3D11_MAP flag = D3D11_MAP_READ);
		void* Lock(cRenderer &renderer, OUT D3D11_MAPPED_SUBRESOURCE &out);
		void Unlock(cRenderer &renderer);


	public:
		ID3D11Buffer *m_buff;
		int m_stride; // byte size of element
		int m_elementCount;
	};
}
