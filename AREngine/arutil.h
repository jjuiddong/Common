//
//
// ARToolkit에서 필요한 유틸리티 함수를 모았다.
//
//
#pragma once


namespace ar
{
	
	struct sQRPos
	{
		bool used;
		cv::Point pos;
	};

	void D3DConvMatrixView(float* src, D3DXMATRIXA16* dest);
	void D3DConvMatrixProjection(float* src, D3DXMATRIXA16* dest);

}

