// 게임에서 사용될 리소스를 관리하는 클래스
// 재활용 할 수 있는 리소스를 다시 로딩하지 않고 쓸 수 있게 한다.
// 재활용 리소스
// - model
// - animation
// - texture
// - shader
#pragma  once


namespace graphic
{
	struct sRawMesh;
	struct sRawAni;
	struct sRawBone;
	class cXFileMesh;
	class cColladaModel;


	class cResourceManager : public common::cSingleton<cResourceManager>
	{
	public:
		cResourceManager();
		virtual ~cResourceManager();
		
		sRawMeshGroup* LoadRawMesh( const string &fileName );
		bool InsertRawMesh(sRawMeshGroup *meshes);
		sRawMeshGroup2* LoadRawMesh2(const string &fileName);

		cXFileMesh* LoadXFile(cRenderer &renderer, const string &fileName);
		cColladaModel * LoadColladaModel(cRenderer &renderer, const string &fileName);

		std::pair<bool, cXFileMesh*> LoadXFileParallel(cRenderer &renderer, const string &fileName);
		void InsertXFileModel(const string &fileName, cXFileMesh *p);

		std::pair<bool, cColladaModel*> LoadColladaModelParallel(cRenderer &renderer, const string &fileName);
		void InsertColladaModel(const string &fileName, cColladaModel *p);

		cShadowVolume* LoadShadow(cRenderer &renderer, const string &fileName);
		std::pair<bool, cShadowVolume*> LoadShadowParallel(cRenderer &renderer, const string &fileName);
		void InsertShadow(const string &fileName, cShadowVolume *p);


		sRawAniGroup* LoadAnimation( const string &fileName );
		bool LoadAnimation(sRawAniGroup *anies);
		cMeshBuffer* LoadMeshBuffer(cRenderer &renderer, const string &meshName);
		cMeshBuffer* LoadMeshBuffer(cRenderer &renderer, const sRawMesh &rawMesh);
		cTexture* LoadTexture(cRenderer &renderer, const string &fileName, const bool isSizePow2 = true, const bool isRecursive=true);
		cTexture* LoadTexture(cRenderer &renderer, const string &dirPath, const string &fileName, const bool isSizePow2 = true, const bool isRecursive = true);
		cTexture* LoadTexture2(cRenderer &renderer, const string &dirPath, const string &fileName, const bool isSizePow2 = true, const bool isRecursive = true);
		cCubeTexture* LoadCubeTexture(cRenderer &renderer, const string &fileName, const bool isSizePow2 = true, const bool isRecursive = true);
		cShader* LoadShader(cRenderer &renderer, const string &fileName);

		sRawMeshGroup* FindModel( const string &fileName );
		sRawMeshGroup2* FindModel2(const string &fileName);
		std::pair<bool, cXFileMesh*> FindXFile(const string &fileName);
		cColladaModel * FindColladaModel(const string &fileName);
		std::pair<bool, cShadowVolume*> FindShadow(const string &fileName);

		sRawAniGroup* FindAnimation( const string &fileName );
		cMeshBuffer* FindMeshBuffer( const string &meshName );
		cTexture* FindTexture( const string &fileName );
		cCubeTexture* FindCubeTexture(const string &fileName);
		cShader * FindShader(const string &fileName);
		string FindFile( const string &fileName );
		string GetResourceFilePath(const string &fileName);
		string GetResourceFilePath(const string &dir, const string &fileName);


		void SetMediaDirectory( const string &path );
		const string& GetMediaDirectory() const;
		string GetRelativePathToMedia( const string &fileName );
		void ReloadShader(cRenderer &renderer);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();

		RESOURCE_TYPE::TYPE GetFileKind( const string &fileName );


	private:
		CriticalSection m_cs;
		CriticalSection m_csShadow;

		map<string, sRawMeshGroup*> m_meshes; // key = fileName
		map<string, sRawMeshGroup2*> m_meshes2; // key = fileName
		map<string, cXFileMesh*> m_xfiles; // key = fileName
		map<string, cColladaModel*> m_colladaModels; // key = fileName
		map<string, cShadowVolume*> m_shadows; // key = fileName

		map<string, sRawAniGroup*> m_anies;	// key = fileName
		map<string, cMeshBuffer*> m_mesheBuffers; // key = meshName
		map<string, cTexture*> m_textures; // key = fileName
		map<string, cCubeTexture*> m_cubeTextures; // key = fileName
		map<string, cShader*> m_shaders; // key = fileName
		string m_mediaDirectory; // default : ../media/

		cThread m_loadThread;
		int m_loadId;
	};


	inline void cResourceManager::SetMediaDirectory( const string &path ) { m_mediaDirectory = path; }
	inline const string& cResourceManager::GetMediaDirectory() const { return m_mediaDirectory; }
}
