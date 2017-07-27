#pragma once


namespace graphic
{

	class cTerrainEditor : public cTerrain
	{
	public:
		//cTerrainEditor(cRenderer &renderer);
		cTerrainEditor();
		virtual ~cTerrainEditor();

		bool Init(cRenderer &renderer);
		bool CreateFromGRDFile(cRenderer &renderer, const StrPath &fileName);
		bool WriteGRDFile( const StrPath &fileName );
		bool WriteTRNFile( const StrPath &fileName );
		bool WriteTerrainTextureToPNGFile( cRenderer &renderer, const StrPath &fileName );
		void GenerateRawTerrain( OUT sRawTerrain &out );

		void BrushTerrain( const cTerrainCursor &cursor, const float elapseT );
		void BrushTexture( const cTerrainCursor &cursor );

		void SetHeightFactor(const float heightFactor);
		void SetTextureUVFactor(const float textureUVFactor);


	protected:
		void GetTextureUV(const Vector3 &pos, OUT float &tu, OUT float &tv);

	};

}
