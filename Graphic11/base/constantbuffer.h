//
// 2017-07-28, jjuiddong
// DX11 constant buffer
//
#pragma once


namespace graphic
{

	class cConstantBuffer
	{
	public:
		cConstantBuffer();
		virtual ~cConstantBuffer();

		bool Create(cRenderer &renderer, const int byteSize);
		bool Update(cRenderer &renderer, const void *ptr);
		bool Bind(cRenderer &renderer, const int slot=0);
		bool UpdateAndBind(cRenderer &renderer, const void *ptr);
		void Clear();


	public:
		ID3D11Buffer *m_constantBuffer;
	};

}
