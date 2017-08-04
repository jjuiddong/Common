//
// 2017-06-26, jjuiddong
//
//
#pragma once


namespace common
{

	struct Transform
	{
		Transform();

		Matrix44 GetMatrix() const;

#ifdef USE_D3D11_MATH
		XMMATRIX GetMatrixXM() const;
#endif


		Vector3 pos;
		Vector3 scale;
		Quaternion rot;
		const static Transform Identity;
	};

}
