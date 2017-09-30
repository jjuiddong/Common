//
// 2017-07-10, jjuiddong
// json format 
//
// 2017-08-21
//	- DX11 Upgrade
//
#pragma once


namespace graphic
{

	class cTerrainLoader
	{
	public:
		cTerrainLoader(cTerrain2 *terrain);
		virtual ~cTerrainLoader();

		bool Write(const StrPath &fileName);
		bool Read(cRenderer &renderer, const StrPath &fileName);


	protected:
		bool WriteHeightmap(const char *fileName);
		bool ReadHeightmap(const char *fileName);


	public:
		cTerrain2 *m_terrain; // reference
	};
}

