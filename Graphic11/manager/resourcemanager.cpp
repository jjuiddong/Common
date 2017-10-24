
#include "stdafx.h"
#include "resourcemanager.h"
#include "task_resource.h"


using namespace graphic;


cResourceManager::cResourceManager() 
	: m_mediaDirectory("../media/")
	, m_loadId(0)
{
}

cResourceManager::~cResourceManager()
{
	Clear();
}


void cResourceManager::Update(const float deltaSeconds)
{
	// Check Parallel Loader 
	int cnt = 0;
	cParallelLoader *tmp[4] = { NULL, NULL, NULL, NULL };

	for (auto &p : m_ploaders)
	{
		if (p->Update(deltaSeconds))
			tmp[cnt++] = p;
		if (cnt >= 4)
			break;
	}

	for (int i = 0; i < cnt; ++i)
	{
		common::popvector2(m_ploaders, tmp[i]);
		delete tmp[i];
	}
	//

}


// load Assimp model file
sRawMeshGroup2* cResourceManager::LoadRawMesh(const StrPath &fileName)
{
	RETV(fileName.empty(), NULL);

	if (sRawMeshGroup2 *data = FindModel2(fileName.c_str()))
		return data;

	cAssimpLoader loader;
	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	if (!loader.Create(resourcePath))
		goto error;

	if (loader.m_rawMeshes)
		m_meshes2[fileName.GetHashCode()] = loader.m_rawMeshes;

	if (loader.m_rawAnies && !loader.m_rawAnies->anies.empty())
	{
		//loader.m_rawMeshes->animationName = loader.m_rawAnies->anies[0].name;
		//m_anies[loader.m_rawAnies->name.GetHashCode()] = loader.m_rawAnies;
		loader.m_rawMeshes->animationName = fileName.c_str();
		m_anies[fileName.GetHashCode()] = loader.m_rawAnies;
		ReadSequenceFile(resourcePath, *loader.m_rawAnies);
	}

	return loader.m_rawMeshes;


error:
	dbg::ErrLog("Err LoadRawMesh2 %s \n", fileName.c_str());
	return NULL;
}


// read '.seq' file
// animation name, start time, end time
bool cResourceManager::ReadSequenceFile(const StrPath &modelFileName, sRawAniGroup &rawAnies)
{
	RETV(rawAnies.anies.empty(), false);

	using namespace std;
	StrPath aniFileName = modelFileName.GetFileNameExceptExt2() + ".seq";
	ifstream ifs(aniFileName.c_str());
	if (!ifs.is_open())
		return false;

	sRawAni &originalAni = rawAnies.anies[0];

	Str128 line;
	while (ifs.getline(line.m_str, sizeof(line.m_str)))
	{
		stringstream ss(line.c_str());
		StrId name;
		float start = -1, end = -1;
		ss >> name.m_str >> start >> end;

		if (name.empty())
			continue;

		sRawAni rawAni;

		// check animation frame
		if ((originalAni.start > start) ||
			(originalAni.end < end))
			continue; // not found animation from original

		int posCnt = 0;
		int rotCnt = 0;
		int scaleCnt = 0;
	}

	return true;
}


// Load Assimp Model
cAssimpModel* cResourceManager::LoadAssimpModel( cRenderer &renderer
	, const StrPath &fileName
)
{
	auto result = FindAssimpModel(fileName);
	if (result.first)
		return result.second;

	cAssimpModel *model = NULL;
	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	{
		AutoCSLock cs(m_cs);
		model = new cAssimpModel;
		if (!model->Create(renderer, resourcePath))
			goto error;
		
		m_assimpModels[fileName.GetHashCode()] = model;
		return model;
	}


error:
	dbg::ErrLog("Err LoadAssimpModel %s \n", fileName.c_str());
	SAFE_DELETE(model);
	return NULL;
}


// Load Parallel Assimp file
std::pair<bool, cAssimpModel*> cResourceManager::LoadAssimpModelParallel(cRenderer &renderer, const StrPath &fileName)
{
	AutoCSLock cs(m_cs);

	auto result = FindAssimpModel(fileName);
	if (result.first)
		return{ true, result.second };

	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	AddTask( new cTaskAssimpLoader(0, &renderer, fileName) );
	m_assimpModels[fileName.GetHashCode()] = NULL;

	return{ true, NULL };


error:
	dbg::ErrLog("Err LoadAssimpModelParallel %s \n", fileName.c_str());
	return{ false, NULL };
}


// Set Model Pointer If Finish Parallel Load 
void cResourceManager::InsertAssimpModel(const StrPath &fileName, cAssimpModel *p)
{
	AutoCSLock cs(m_cs);
	m_assimpModels[fileName.GetHashCode()] = p;
}


// 애니메이션 파일 로딩.
sRawAniGroup* cResourceManager::LoadAnimation( const StrId &fileName )
{
	RETV(fileName.empty(), NULL);

	if (sRawAniGroup *data = FindAnimation(fileName))
		return data;

	// Animation Read only assimp loader
	goto error;

error:
	dbg::ErrLog("Err LoadAnimation %s \n", fileName.c_str());
	return NULL;
}


// Register Animation Information
bool cResourceManager::LoadAnimation(const StrPath &fileName, sRawAniGroup *anies)
{
	RETV(!anies, false);

	m_anies[fileName.GetHashCode()] = anies;
	return true;
}


// find model data
sRawMeshGroup2* cResourceManager::FindModel2(const StrId &fileName)
{
	auto it = m_meshes2.find(fileName.GetHashCode());
	if (m_meshes2.end() == it)
		return NULL; // not exist
	return it->second;
}


std::pair<bool, cAssimpModel*> cResourceManager::FindAssimpModel(const StrPath &fileName)
{
	AutoCSLock cs(m_cs);
	auto it = m_assimpModels.find(fileName.GetHashCode());
	if (m_assimpModels.end() != it)
		return{ true, it->second };
	return{ false, NULL };
}


// find animation data
sRawAniGroup* cResourceManager::FindAnimation( const StrId &fileName )
{
	auto it = m_anies.find(fileName.GetHashCode());
	if (m_anies.end() == it)
		return NULL; // not exist
	return it->second;
}


// 텍스쳐 로딩.
cTexture* cResourceManager::LoadTexture(cRenderer &renderer, const StrPath &fileName
	, const bool isRecursive //= true
)
{
	auto result = FindTexture(fileName);
	if (result.first)
		return result.second;

	cTexture *texture = NULL;
	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	texture = new cTexture();
	if (!texture->Create(renderer, resourcePath))
	{
		if (fileName == g_defaultTexture) // this file must loaded
		{
			assert(0);
			goto error;
		}
	}

	if (texture && texture->IsLoaded())
	{
		AutoCSLock cs(m_cs);
		m_textures[fileName.GetHashCode()] = texture;
		return texture;
	}


error:
	dbg::ErrLog("Error LoadTexture1 %s \n", fileName.c_str());
	SAFE_DELETE(texture);
	if (isRecursive)
		return cResourceManager::LoadTexture(renderer, g_defaultTexture, false);
	return NULL;
}


// Parallel Load Texture
cTexture* cResourceManager::LoadTextureParallel(cRenderer &renderer, const StrPath &fileName
	, const bool isSizePow2 //= true
)
{
	auto result = FindTexture(fileName);
	if (result.first)
		return result.second;

	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;
	
	AddTask(new cTaskTextureLoader(0, &renderer, fileName, resourcePath));

	{
		AutoCSLock cs(m_cs);
		m_textures[fileName.GetHashCode()] = NULL;
		return NULL;
	}


error:
	dbg::ErrLog("Error LoadTextureParallel %s \n", fileName.c_str());
	return NULL;
}


void cResourceManager::InsertTexture(const StrPath &fileName, cTexture *p)
{
	AutoCSLock cs(m_cs);
	m_textures[fileName.GetHashCode()] = p;
}


// 텍스쳐 로딩.
//cCubeTexture* cResourceManager::LoadCubeTexture(cRenderer &renderer, const StrPath &fileName
//	, const bool isSizePow2 //=true
//	, const bool isRecursive //= true
//)
//{
//	if (cCubeTexture *p = FindCubeTexture(fileName))
//		return p;
//
//	cCubeTexture *texture = NULL;
//	const StrPath resourcePath = GetResourceFilePath(fileName);
//	if (resourcePath.empty())
//		goto error;
//
//	texture = new cCubeTexture();
//	if (!texture->Create(renderer, resourcePath))
//		goto error;
//
//	if (texture && texture->IsLoaded())
//	{
//		m_cubeTextures[fileName.GetHashCode()] = texture;
//		return texture;
//	}
//
//
//error:
//	dbg::ErrLog("Err LoadCubeTexture %s \n", fileName.c_str());
//	SAFE_DELETE(texture);
//	return NULL;
//}


// 텍스쳐 로딩.
// fileName 에 해당하는 파일이 없다면, "../media/" + dirPath  경로에서 파일을 찾는다.
cTexture* cResourceManager::LoadTexture(cRenderer &renderer, const StrPath &dirPath, const StrPath &fileName
	, const bool isRecursive //= true
)
{
	auto result = FindTexture(fileName);
	if (result.first)
		return result.second;

	StrPath key = fileName;
	StrPath cvtFileName = cTexture::ConvertTextureFileName(fileName.c_str());
	cTexture *texture = NULL;
	if (common::IsFileExist(cvtFileName))
	{
		texture = new cTexture();
		texture->Create(renderer, cvtFileName);
	}
	else
	{
		StrPath newPath;
		StrPath searchPath = m_mediaDirectory + dirPath;
		if (searchPath.empty())
			searchPath = ".";

		key = newPath;
		if (common::FindFile(cvtFileName.GetFileName(), searchPath + "/", newPath))
		{
			if (isRecursive)
			{
				if (texture = cResourceManager::LoadTexture(renderer, newPath, false))
					return texture;
			}
			else
			{
				if (common::IsFileExist(newPath))
				{
					texture = new cTexture();
					texture->Create(renderer, newPath);
				}
			}
		}
	}

	if (texture && texture->IsLoaded())
	{
		AutoCSLock cs(m_cs);
		m_textures[key.GetHashCode()] = texture;
		return texture;
	}
	else
	{
		dbg::ErrLog("Error LoadTexture %s \n", fileName.c_str());

		// load error
		if (!texture)
		{  // Not Exist File
			return cResourceManager::LoadTexture(renderer, g_defaultTexture, false);
		}

		// last load g_defaultTexture texture
		if (!texture->IsLoaded())
		{ // File Exist, but Load Error
			delete texture;
			return cResourceManager::LoadTexture(renderer, g_defaultTexture, false);
		}
	}

	return NULL;
}


// 텍스쳐 로딩.
// fileName 에 해당하는 파일이 없다면, dirPath 경로에서 파일을 찾는다.
cTexture* cResourceManager::LoadTexture2(cRenderer &renderer, const StrPath &dirPath, const StrPath &fileName
	, const bool isRecursive //= true
)
{
	auto result = FindTexture(fileName);
	if (result.first)
		return result.second;

	StrPath cvtFileName = cTexture::ConvertTextureFileName(fileName.c_str());

	cTexture *texture = NULL;
	const StrPath resourcePath = GetResourceFilePath(dirPath, cvtFileName);
	if (resourcePath.empty())
		goto error;

	texture = new cTexture();
	if (!texture->Create(renderer, resourcePath))
		goto error;

	if (texture)
	{
		AutoCSLock cs(m_cs);
		m_textures[fileName.GetHashCode()] = texture;
	}

	return texture;


error:
	dbg::ErrLog("Error LoadTexture2 %s \n", fileName.c_str());
	SAFE_DELETE(texture);
	if (isRecursive)
		return cResourceManager::LoadTexture(renderer, g_defaultTexture, false);
	return NULL;
}


// 텍스쳐 찾기.
std::pair<bool,cTexture*> cResourceManager::FindTexture( const StrPath &fileName )
{
	AutoCSLock cs(m_cs);
	auto it = m_textures.find(fileName.GetHashCode());
	if (m_textures.end() == it)
		return{ false, NULL }; // not exist
	return{ true, it->second };
}

//
//cCubeTexture* cResourceManager::FindCubeTexture(const StrPath &fileName)
//{
//	auto it = m_cubeTextures.find(fileName.GetHashCode());
//	if (m_cubeTextures.end() == it)
//		return NULL; // not exist
//	return it->second;
//}
//

// media 폴더내에 fileName 에 해당하는 파일이 존재한다면,
// 전체 경로를 리턴한다. 파일을 찾을 때는 파일이름만 비교한다.
StrPath cResourceManager::FindFile( const StrPath &fileName )
{
	StrPath newPath;
	if (common::FindFile(fileName, m_mediaDirectory, newPath))
	{
		return newPath;
	}
	return ""; //empty string
}


void cResourceManager::AddParallelLoader(cParallelLoader *p)
{
	m_ploaders.push_back(p);
}


void cResourceManager::AddTask(cTask *task)
{
	task->m_Id = ++m_loadId;
	m_loadThread.AddTask(task);
	if (!m_loadThread.IsRun())
		m_loadThread.Start();
}


//
// media 폴더내에 fileName이 있으면, 경로를 리턴한다.
StrPath cResourceManager::GetResourceFilePath(const StrPath &fileName)
{
	return GetResourceFilePath(m_mediaDirectory, fileName);
}


// dir 폴더내에 fileName이 있으면, 경로를 리턴한다.
// 하위 5단계의 디렉토리까지만 검사한다.
StrPath cResourceManager::GetResourceFilePath(const StrPath &dir, const StrPath &fileName)
{
	if (common::IsFileExist(fileName))
	{
		return fileName;
	}
	else
	{
		const StrPath composeMediaFileName = dir + fileName;
		if (composeMediaFileName.IsFileExist())
		{
			return composeMediaFileName;
		}
		else
		{
			StrPath newPath;
			StrPath searchPath = dir;
			if (searchPath.empty())
				searchPath = ".";
			if ((searchPath.back() != '/') || (searchPath.back() != '\\'))
				searchPath += "/";

			StrPath onlyFileName = fileName.GetFileName();
			if (common::FindFile(onlyFileName, searchPath, newPath, 5))
			{
				return newPath;
			}
		}
	}
	return "";
}


// remove all data
void cResourceManager::Clear()
{
	m_loadThread.Clear();

	// remove raw mesh2
	for each (auto kv in m_meshes2)
	{
		delete kv.second;
	}
	m_meshes2.clear();

	// remove raw assimp models
	{
		AutoCSLock cs(m_cs);
		for each (auto kv in m_assimpModels)
		{
			delete kv.second;
		}
		m_assimpModels.clear();
	}

	// remove texture
	{
		const StrPath defTex = g_defaultTexture;
		AutoCSLock cs(m_cs);
		cTexture *defaultTex = m_textures[defTex.GetHashCode()];
		for each (auto kv in m_textures)
		{
			if (kv.second != defaultTex)
				delete kv.second;
		}
		SAFE_DELETE(defaultTex);
		m_textures.clear();
	}

//	// remove cube texture
//	for each (auto kv in m_cubeTextures)
//	{
//		delete kv.second;
//	}
//	m_cubeTextures.clear();
//

	// remove raw ani
	for each (auto kv in m_anies)
	{
		delete kv.second;
	}
	m_anies.clear();
}


// 파일경로 fileName을 media 폴더의 상대 주소로 바꿔서 리턴한다.
// ex)
// media : c:/project/media,  
// fileName : c:/project/media/terrain/file.txt
// result = ./terrain/file.txt
StrPath cResourceManager::GetRelativePathToMedia( const StrPath &fileName )
{
	const StrPath relatePath = common::RelativePathTo(m_mediaDirectory.GetFullFileName(), fileName.GetFullFileName());
	return relatePath;
}
