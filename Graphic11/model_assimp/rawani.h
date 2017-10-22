#pragma once


namespace graphic
{

	struct sKeyPos
	{
		float t; // frame 단위
		Vector3 p;
	};


	struct sKeyScale
	{
		float t; // frame 단위
		Vector3 s;
	};


	struct sKeyRot
	{
		float t; // frame 단위
		Quaternion q;
	};


	struct sBoneAni
	{
		Str32 name; // Bone Name
		float start; // seconds unit
		float end; // seconds unit
		vector<sKeyPos> pos; // localTm Animation
		vector<sKeyRot> rot; // localTm Animation
		vector<sKeyScale> scale; // localTm Animation
	};


	struct sRawAni
	{
		Str64 name;
		float start; // seconds unit
		float end; // seconds unit
		vector<sBoneAni> boneAnies; // Bone Animation Array, index = bone index
	};


	// 모델에서 하나의 애니메이션 정보를 저장하는 자료구조.
	struct sRawAniGroup
	{
		vector<sRawAni> anies; // Animation Array
	};

}
