#pragma once

namespace graphic
{

	// 다이렉트 X 초기화.
	bool InitDirectX(HWND hWnd, const int width, const int height, const UINT adapter,
		OUT D3DPRESENT_PARAMETERS &param, OUT LPDIRECT3DDEVICE9 &pDevice);


}
