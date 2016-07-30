
#include "stdafx.h"
#include "arutil.h"

using namespace ar;


// 모델뷰 행렬(OpenGL) -> 뷰 행렬(Direct3D)
// http://www.cg-ya.net/imedia/ar/artoolkit_directx/
void ar::D3DConvMatrixView(float* src, D3DXMATRIXA16* dest)
{
	// src의 값을 dest로 1:1 복사를 수행한다.
	dest->_11 = src[0];
	dest->_12 = -src[1];
	dest->_13 = src[2];
	dest->_14 = src[3];

	dest->_21 = src[8];
	dest->_22 = -src[9];
	dest->_23 = src[10];
	dest->_24 = src[7];

	dest->_31 = src[4];
	dest->_32 = -src[5];
	dest->_33 = src[6];
	dest->_34 = src[11];

	dest->_41 = src[12];
	dest->_42 = -src[13];
	dest->_43 = src[14];
	dest->_44 = src[15];
}


// 투영 행렬(OpenGL) -> 투영 행렬(Direct3D)
// http://www.cg-ya.net/imedia/ar/artoolkit_directx/
void ar::D3DConvMatrixProjection(float* src, D3DXMATRIXA16* dest)
{
	dest->_11 = src[0];
	dest->_12 = src[1];
	dest->_13 = src[2];
	dest->_14 = src[3];
	dest->_21 = src[4];
	dest->_22 = src[5];
	dest->_23 = src[6];
	dest->_24 = src[7];
	dest->_31 = -src[8];
	dest->_32 = -src[9];
	dest->_33 = -src[10];
	dest->_34 = -src[11];
	dest->_41 = src[12];
	dest->_42 = src[13];
	dest->_43 = src[14];
	dest->_44 = src[15];
}

