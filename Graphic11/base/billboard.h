//
// 2017-08-08, jjuiddong
//	- Upgrade DX11
//		빌보드 클래스.
//		해골 책 빌보드 아티클을 참조함.
//
#pragma once


namespace graphic
{

	// 빌보드 형태 값.
	namespace BILLBOARD_TYPE {
		enum TYPE
		{
			NONE,
			Y_AXIS,		// Y Axis Rotation
			ALL_AXIS,	// X-Y-Z Axis Rotation
			ALL_AXIS_Z, // X-Y-Z Axis Rotation and Z Axis is Up
			DYN_SCALE,
		};
	};


	class cBillboard : public cQuad
	{
	public:		
		cBillboard();
		virtual ~cBillboard();

		bool Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type, const float width, const float height,
			const Vector3 &pos, const StrPath &textureFileName = "", const bool isSizePow2=true);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;
		void Rotate();


	public:
		BILLBOARD_TYPE::TYPE m_type;
		Vector3 m_scale;
		Vector3 m_normal;
	};

}
