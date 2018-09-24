//
// 2017-07-28, jjuiddong
// DX11 constant buffer
//
#pragma once


namespace graphic
{
	using namespace common;

	template<typename T>
	class cConstantBuffer
	{
	public:
		cConstantBuffer()
			: m_constantBuffer(NULL) {
		}

		~cConstantBuffer() {
			Clear();
		}

		bool Create(iRenderer &renderer)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.ByteWidth = sizeof(T);
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, NULL, &m_constantBuffer)))
				return false;

			m_v = (T*)_aligned_malloc(sizeof(T), 16);

			return true;
		}

		
		bool Update(iRenderer &renderer, const int slot = 0)
		{
			RETV(!m_constantBuffer, false);
			renderer.GetDevContext()->UpdateSubresource(m_constantBuffer, 0, NULL, m_v, 0, 0);
			renderer.GetDevContext()->VSSetConstantBuffers(slot, 1, &m_constantBuffer);
			renderer.GetDevContext()->GSSetConstantBuffers(slot, 1, &m_constantBuffer);
			renderer.GetDevContext()->HSSetConstantBuffers(slot, 1, &m_constantBuffer);
			renderer.GetDevContext()->DSSetConstantBuffers(slot, 1, &m_constantBuffer);
			renderer.GetDevContext()->PSSetConstantBuffers(slot, 1, &m_constantBuffer);
			return true;
		}


		void Clear()
		{
			SAFE_RELEASE(m_constantBuffer);
			_aligned_free(m_v);
			m_v = NULL;
		}


		T& operator = (const T& rhs) {
			if (m_v != &rhs) {
				*m_v = rhs;
			}
			return *m_v;
		}

		
	public:
		ID3D11Buffer *m_constantBuffer;
		T *m_v;
	};

}
