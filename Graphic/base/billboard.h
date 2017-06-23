// 빌보드 클래스.
// 해골 책 빌보드 아티클을 참조함.
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
		};
	};


	class cBillboard : public cQuad
	{
	public:		
		cBillboard();
		virtual ~cBillboard();

		bool Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type, const float width, const float height,
			const Vector3 &pos, const string &textureFileName = "", const bool isSizePow2=true);
		void Render(cRenderer &renderer, const Vector3 &parentPos = Vector3(0, 0, 0));
		void RenderFactor(cRenderer &renderer, const Vector3 &parentPos=Vector3(0,0,0));
		void Rotate(const Vector3 &parentPos=Vector3(0,0,0));


	private:
		BILLBOARD_TYPE::TYPE m_type;
	};

}
