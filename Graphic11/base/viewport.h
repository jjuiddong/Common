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

		const cViewport& operator=(const D3D11_VIEWPORT &vp);		


	public:
		D3D11_VIEWPORT m_vp;
	};


	inline float cViewport::GetX() const {return m_vp.TopLeftX;}
	inline float cViewport::GetY() const {return m_vp.TopLeftY;}
	inline float cViewport::GetMinZ() const {return m_vp.MinDepth;}
	inline float cViewport::GetMaxZ() const {return m_vp.MaxDepth;}
	inline float cViewport::GetWidth() const {return m_vp.Width;}
	inline float cViewport::GetHeight() const {return m_vp.Height;}
	inline const cViewport& cViewport::operator=(const D3D11_VIEWPORT &vp) { m_vp = vp; return *this; }
}
