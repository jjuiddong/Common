#pragma once


namespace graphic
{

	struct sRawTerrrainModel
	{
		StrPath fileName;
		Matrix44 tm;
	};

	struct sRawLayer
	{
		StrPath texture;
	};


	struct sRawTerrain
	{
		int colCellCount;
		int rowCellCount;
		float cellSize;
		int heightMapStyle; // 0=heightMap, 1=grid format
		StrPath heightMap;
		StrPath bgTexture;
		StrPath alphaTexture;
		int alphaTextureWidth;
		int alphaTextureHeight;
		float textureFactor;
		float heightFactor;
		bool renderWater;
		sRawLayer layer[4];
		vector<sRawTerrrainModel> models;
	};


}
