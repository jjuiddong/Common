#pragma once


namespace graphic 
{ 


	struct sRawTerrain;

	namespace importer 
	{

		bool ReadRawTerrainFile( const StrPath &fileName, OUT sRawTerrain &out );

	}


}