//
// 2019-12-15, jjuiddong
// share buffer, CPU-GPU
//
#pragma once


namespace graphic
{
	using namespace common;

	class cShareBuffer
	{
	public:
		cShareBuffer();
		virtual ~cShareBuffer();

		bool Create(cRenderer &renderer, const void *pInitMem
			, const int stride, const int count
			, const bool isShaderResource = false
			, const bool isUnorderedAccess = false);
		bool CreateReadBuffer(cRenderer &renderer, const cShareBuffer &src);

		bool Copy(cRenderer &renderer, const cShareBuffer &src);
		bool Bind(cRenderer &renderer, const int stage=0);
		bool BindUnorderedAccessView(cRenderer &renderer, const int stage = 0);
		void* Lock(cRenderer &renderer, const D3D11_MAP flag = D3D11_MAP_READ);
		void* Lock(cRenderer &renderer, OUT D3D11_MAPPED_SUBRESOURCE &out);
		void Unlock(cRenderer &renderer);
		void Clear();


	protected:
		bool CreateShaderResourceView(cRenderer &renderer, const D3D11_BUFFER_DESC &desc);
		bool CreateUnorderedAccessView(cRenderer &renderer, const D3D11_BUFFER_DESC &desc);


	public:
		int m_stride; // byte size of element
		int m_count; // element count
		ID3D11Buffer *m_buff;
		ID3D11ShaderResourceView *m_srv;
		ID3D11UnorderedAccessView *m_uav;
	};
}
