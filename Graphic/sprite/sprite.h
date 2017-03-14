//
// 2017-02-23, jjuiddong
// Management LPD3DXSPRITE
//
#pragma once


namespace graphic
{
	class cSprite
	{
	public:
		cSprite();
		virtual ~cSprite();
		bool Create(cRenderer &renderer);
		void Begin();
		void Render(cTexture &texture, const sRect &rect, const Vector3 &center, 
			const D3DCOLOR color, const Matrix44 &tm = Matrix44::Identity);
		void End();
		void LostDevice();
		void ResetDevice(graphic::cRenderer &renderer);
		void Clear();


	public:
		LPD3DXSPRITE m_p;
		static cSprite NullSprite;
	};

}
