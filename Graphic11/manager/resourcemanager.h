//
// 2017-08-10, jjuiddong
//	- Upgrade DX11
//	게임에서 사용될 리소스를 관리하는 클래스
//	재활용 할 수 있는 리소스를 다시 로딩하지 않고 쓸 수 있게 한다.
//	재활용 리소스
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
	class cAssimpModel;
	class cParallelLoader;


	class cResourceManager : public common::cSingleton<cResourceManager>
	{
	public:
		cResourceManager();
		virtual ~cResourceManager();

		void Update(const float deltaSeconds);
		
		sRawMeshGroup2* LoadRawMesh2(const StrPath &fileName);
		cAssimpModel * LoadAssimpModel(cRenderer &renderer, const StrPath &fileName);

		std::pair<bool, cAssimpModel*> LoadAssimpModelParallel(cRenderer &renderer, const StrPath &fileName);
		void InsertAssimpModel(const StrPath &fileName, cAssimpModel *p);

		//cShadowVolume* LoadShadow(cRenderer &renderer, const StrPath &fileName);
		//std::pair<bool, cShadowVolume*> LoadShadowParallel(cRenderer &renderer, const StrPath &fileName);
		//void InsertShadow(const StrPath &fileName, cShadowVolume *p);

		//sRawAniGroup* LoadAnimation( const StrId &fileName );
		bool LoadAnimation(sRawAniGroup *anies);
		cTexture* LoadTexture(cRenderer &renderer, const StrPath &fileName, const bool isRecursive=true);
		cTexture* LoadTexture(cRenderer &renderer, const StrPath &dirPath, const StrPath &fileName, const bool isRecursive = true);
		cTexture* LoadTexture2(cRenderer &renderer, const StrPath &dirPath, const StrPath &fileName, const bool isRecursive = true);
		cTexture* LoadTextureParallel(cRenderer &renderer, const StrPath &fileName, const bool isSizePow2 = true);
		void InsertTexture(const StrPath &fileName, cTexture *p);

		//cCubeTexture* LoadCubeTexture(cRenderer &renderer, const StrPath &fileName, const bool isSizePow2 = true, const bool isRecursive = true);
		//cShader* LoadShader(cRenderer &renderer, const StrPath &fileName);

		sRawMeshGroup2* FindModel2(const StrId &fileName);
		std::pair<bool, cAssimpModel*> FindAssimpModel(const StrPath &fileName);
		//std::pair<bool, cShadowVolume*> FindShadow(const StrPath &fileName);

		sRawAniGroup* FindAnimation( const StrId &fileName );
		std::pair<bool, cTexture*> FindTexture( const StrPath &fileName );
		//cCubeTexture* FindCubeTexture(const StrPath &fileName);
		//cShader * FindShader(const StrPath &fileName);
		StrPath FindFile( const StrPath &fileName );

		void AddParallelLoader(cParallelLoader *p);
		void AddTask(cTask *task);

		void SetMediaDirectory( const StrPath &path );
		const StrPath& GetMediaDirectory() const;
		StrPath GetRelativePathToMedia( const StrPath &fileName );
		//void ReloadShader(cRenderer &renderer);
		StrPath GetResourceFilePath(const StrPath &fileName);
		StrPath GetResourceFilePath(const StrPath &dir, const StrPath &fileName);
		//void LostDevice();
		//void ResetDevice(cRenderer &renderer);
		void Clear();


	private:
		CriticalSection m_cs;
		CriticalSection m_csShadow;

		map<hashcode, sRawMeshGroup2*> m_meshes2; // key = fileName
		map<hashcode, cAssimpModel*> m_assimpModels; // key = fileName
		//map<hashcode, cShadowVolume*> m_shadows; // key = fileName

		map<hashcode, sRawAniGroup*> m_anies;	// key = fileName
		map<hashcode, cTexture*> m_textures; // key = fileName
		//map<hashcode, cCubeTexture*> m_cubeTextures; // key = fileName
		//map<hashcode, cShader*> m_shaders; // key = fileName
		StrPath m_mediaDirectory; // default : ../media/

		vector<cParallelLoader*> m_ploaders;

		cThread m_loadThread;
		int m_loadId;
	};


	inline void cResourceManager::SetMediaDirectory( const StrPath &path ) { m_mediaDirectory = path; }
	inline const StrPath& cResourceManager::GetMediaDirectory() const { return m_mediaDirectory; }
}
