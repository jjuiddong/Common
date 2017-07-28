//
// 2017-07-28, jjuiddong
// DX11 Viewport Class
//
#pragma once


namespace graphic
{

	class cViewport
	{
	public:
		cViewport();
		cViewport(const float x, const float y,
			const float width, const float height,
			const float minZ, const float maxZ);
		virtual ~cViewport();
		
		bool Create(const float x=0, const float y=0,
			const float width=1024, const float height=768,
			const float minZ=1.f, const float maxZ=10000.f);

		bool Bind(cRenderer &renderer);

		float GetX() const;
		float GetY() const;
		float GetMinZ() const;
		float GetMaxZ() const;
		float GetWidth() const;
		float GetHeight() const;


	public:
		D3D11_VIEWPORT m_vp;
	};

}
