//
// 2017-02-23, jjuiddong
// Sprite class 
//
#pragma once


namespace graphic
{
	class cSprite2 : public cNode
	{
	public:
		cSprite2(cSprite &sprite, const int id, const Str64 &name = "sprite");
		virtual ~cSprite2();

		void SetTexture(cRenderer &renderer, const StrPath &fileName);
		virtual void Render(cRenderer &renderer, const Matrix44 &parentTm=Matrix44::Identity) override;
		void SetCenter(const Vector3 &center); // x,y = 0~1
		bool IsContain(const Vector2 &pos);
		void LostDevice();
		void ResetDevice(graphic::cRenderer &renderer);
		void Clear();


	public:
		cSprite &m_sprite; // reference
		cTexture *m_texture; // reference
		Vector3 m_center;	// x,y = 0~1, default : 0,0
		Vector3 m_pos; // pixel coordinate
		Vector3 m_scale;
		sRecti m_rect; // pixel coordinate
		D3DCOLOR m_color;
		Matrix44 m_accTM;
	};
}
