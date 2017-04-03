#pragma once


namespace graphic
{

	class cViewport
	{
	public:
		cViewport();
		cViewport(const int x, const int y,
			const int width, const int height,
			const float minZ, const float maxZ);
		virtual ~cViewport();
		
		bool Create(const int x=0, const int y=0, 
			const int width=1024, const int height=768, 
			const float minZ=1.f, const float maxZ=10000.f);


	public:
		D3DVIEWPORT9 m_vp;
	};

}