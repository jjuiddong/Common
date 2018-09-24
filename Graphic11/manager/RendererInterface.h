//
// 2018-09-24, jjuiddong
// renderer, constantbuffer mutual reference error solution
//
#pragma once


namespace graphic
{
	interface iRenderer
	{
	public:
		virtual ID3D11Device* GetDevice() = 0;
		virtual ID3D11DeviceContext* GetDevContext() = 0;
	};
}
