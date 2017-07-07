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
	class cParallelLoader;


	class cResourceManager : public common::cSingleton<cResourceManager>
	{
	public:
		cResourceManager();
		virtual ~cResourceManager();

		void Update(const float deltaSeconds);
		
		sRawMeshGroup* LoadRawMesh( const StrPath &fileName );
		bool InsertRawMesh(sRawMeshGroup *meshes);
		sRawMeshGroup2* LoadRawMesh2(const StrPath &fileName);

		cXFileMesh* LoadXFile(cRenderer &renderer, const StrPath &fileName);
		cColladaModel * LoadColladaModel(cRenderer &renderer, const StrPath &fileName);

		std::pair<bool, cXFileMesh*> LoadXFileParallel(cRenderer &renderer, const StrPath &fileName);
		void InsertXFileModel(const StrPath &fileName, cXFileMesh *p);

		std::pair<bool, cColladaModel*> LoadColladaModelParallel(cRenderer &renderer, const StrPath &fileName);
		void InsertColladaModel(const StrPath &fileName, cColladaModel *p);

		cShadowVolume* LoadShadow(cRenderer &renderer, const StrPath &fileName);
		std::pair<bool, cShadowVolume*> LoadShadowParallel(cRenderer &renderer, const StrPath &fileName);
		void InsertShadow(const StrPath &fileName, cShadowVolume *p);


		sRawAniGroup* LoadAnimation( const StrPath &fileName );
		bool LoadAnimation(sRawAniGroup *anies);
		cMeshBuffer* LoadMeshBuffer(cRenderer &renderer, const StrPath &meshName);
		cMeshBuffer* LoadMeshBuffer(cRenderer &renderer, const sRawMesh &rawMesh);
		cTexture* LoadTexture(cRenderer &renderer, const StrPath &fileName, const bool isSizePow2 = true, const bool isRecursive=true);
		cTexture* LoadTexture(cRenderer &renderer, const StrPath &dirPath, const StrPath &fileName, const bool isSizePow2 = true, const bool isRecursive = true);
		cTexture* LoadTexture2(cRenderer &renderer, const StrPath &dirPath, const StrPath &fileName, const bool isSizePow2 = true, const bool isRecursive = true);
		cTexture* LoadTextureParallel(cRenderer &renderer, const StrPath &fileName, const bool isSizePow2 = true);
		void InsertTexture(const StrPath &fileName, cTexture *p);

		cCubeTexture* LoadCubeTexture(cRenderer &renderer, const StrPath &fileName, const bool isSizePow2 = true, const bool isRecursive = true);
		cShader* LoadShader(cRenderer &renderer, const StrPath &fileName);

		sRawMeshGroup* FindModel( const StrPath &fileName );
		sRawMeshGroup2* FindModel2(const StrPath &fileName);
		std::pair<bool, cXFileMesh*> FindXFile(const StrPath &fileName);
		std::pair<bool, cColladaModel*> FindColladaModel(const StrPath &fileName);
		std::pair<bool, cShadowVolume*> FindShadow(const StrPath &fileName);

		sRawAniGroup* FindAnimation( const StrPath &fileName );
		cMeshBuffer* FindMeshBuffer( const StrPath &meshName );
		std::pair<bool, cTexture*> FindTexture( const StrPath &fileName );
		cCubeTexture* FindCubeTexture(const StrPath &fileName);
		cShader * FindShader(const StrPath &fileName);
		StrPath FindFile( const StrPath &fileName );

		void AddParallelLoader(cParallelLoader *p);
		void AddTask(cTask *task);


		void SetMediaDirectory( const StrPath &path );
		const StrPath& GetMediaDirectory() const;
		StrPath GetRelativePathToMedia( const StrPath &fileName );
		void ReloadShader(cRenderer &renderer);
		StrPath GetResourceFilePath(const StrPath &fileName);
		StrPath GetResourceFilePath(const StrPath &dir, const StrPath &fileName);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();

		RESOURCE_TYPE::TYPE GetFileKind( const StrPath &fileName );


	private:
		CriticalSection m_cs;
		CriticalSection m_csShadow;

		map<hashcode, sRawMeshGroup*> m_meshes; // key = fileName
		map<hashcode, sRawMeshGroup2*> m_meshes2; // key = fileName
		map<hashcode, cXFileMesh*> m_xfiles; // key = fileName
		map<hashcode, cColladaModel*> m_colladaModels; // key = fileName
		map<hashcode, cShadowVolume*> m_shadows; // key = fileName

		map<hashcode, sRawAniGroup*> m_anies;	// key = fileName
		map<hashcode, cMeshBuffer*> m_mesheBuffers; // key = meshName
		map<hashcode, cTexture*> m_textures; // key = fileName
		map<hashcode, cCubeTexture*> m_cubeTextures; // key = fileName
		map<hashcode, cShader*> m_shaders; // key = fileName
		StrPath m_mediaDirectory; // default : ../media/

		vector<cParallelLoader*> m_ploaders;

		cThread m_loadThread;
		int m_loadId;
	};


	inline void cResourceManager::SetMediaDirectory( const StrPath &path ) { m_mediaDirectory = path; }
	inline const StrPath& cResourceManager::GetMediaDirectory() const { return m_mediaDirectory; }
}
