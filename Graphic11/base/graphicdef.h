#pragma once


namespace graphic
{

	namespace RESOURCE_TYPE {
		enum TYPE {
			NONE,
			MESH,
			ANIMATION,
		};
	};


	namespace MODEL_TYPE {
		enum TYPE {
			SKIN,
			RIGID,
			AUTO,
		};
	};


	// Matrix44* -> D3DXMATRIX*
	//inline const D3DXMATRIX* ToDxM(const Matrix44 &mat) {
	//	return (const D3DXMATRIX*)&mat;
	//}

	// Vector3* -> D3DVECTOR3*
	//inline const D3DXVECTOR3* ToDxV3(const Vector3 &v) {
	//	return (const D3DXVECTOR3*)&v;
	//}


	// 툴에서 쓰일 모델 변환 정보.
	struct sTransform
	{
		Vector3 pos;
		Vector3 rot; // euler angle
		Vector3 scale;
	};

}


