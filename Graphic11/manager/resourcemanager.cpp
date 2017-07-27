
#include "stdafx.h"
#include "resourcemanager.h"
#include "../importer/modelimporter.h"
#include "../base/material.h"
#include "../model_collada/model_collada.h"
#include "task_resource.h"
#include "../importer/parallelloader.h"


using namespace graphic;


cResourceManager::cResourceManager() :
	m_mediaDirectory("../media/")
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


// load model file
sRawMeshGroup* cResourceManager::LoadRawMesh( const StrPath &fileName )
{
	RETV(fileName.empty(), NULL);

	if (sRawMeshGroup *data = FindModel(fileName.c_str()))
		return data;

	sRawMeshGroup *meshes = NULL;
	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	meshes = new sRawMeshGroup;
	meshes->name = fileName.c_str();

	if (!importer::ReadRawMeshFile(resourcePath.c_str(), *meshes))
		goto error;

	InsertRawMesh(meshes);
	return meshes;


error:
	dbg::ErrLog("Err LoadRawMesh %s \n", fileName.c_str());
	SAFE_DELETE(meshes);
	return NULL;
}


// 외부에서 로딩한 메쉬를 저장한다.
bool cResourceManager::InsertRawMesh(sRawMeshGroup *meshes)
{
	RETV(!meshes, false);

	if (sRawMeshGroup *data = FindModel(meshes->name))
		return false;

	// 메쉬 이름 설정 fileName::meshName
	for (u_int i = 0; i < meshes->meshes.size(); ++i)
	{
		sRawMesh &mesh = meshes->meshes[i];

		mesh.name = meshes->name + "::" + mesh.name;
		if (mesh.mtrlId >= 0)
		{ // 메터리얼 설정.
			mesh.mtrl = meshes->mtrls[mesh.mtrlId];
		}
	}

	m_meshes[meshes->name.GetHashCode()] = meshes;
	return true;
}


// load collada model file
sRawMeshGroup2* cResourceManager::LoadRawMesh2(const StrPath &fileName)
{
	RETV(fileName.empty(), NULL);

	if (sRawMeshGroup2 *data = FindModel2(fileName.c_str()))
		return data;

	cColladaLoader loader;
	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	if (!loader.Create(resourcePath))
		goto error;

	if (loader.m_rawMeshes)
		m_meshes2[fileName.GetHashCode()] = loader.m_rawMeshes;
	if (loader.m_rawAnies)
	{
		loader.m_rawMeshes->animationName = loader.m_rawAnies->name;
		m_anies[loader.m_rawAnies->name.GetHashCode()] = loader.m_rawAnies;
	}
	return loader.m_rawMeshes;


error:
	dbg::ErrLog("Err LoadRawMesh2 %s \n", fileName.c_str());
	return NULL;
}


cXFileMesh* cResourceManager::LoadXFile(cRenderer &renderer, const StrPath &fileName)
{
	auto result = FindXFile(fileName);
	if (result.first)
		return result.second;

	cXFileMesh *mesh = NULL;
	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	mesh = new cXFileMesh;
	if (!mesh->Create(renderer, resourcePath, false, true))
		goto error;

	m_xfiles[fileName.GetHashCode()] = mesh;
	return mesh;


error:
	dbg::ErrLog("Err LoadXFile %s \n", fileName.c_str());
	SAFE_DELETE(mesh);
	return NULL;
}


std::pair<bool, cXFileMesh*> cResourceManager::LoadXFileParallel(cRenderer &renderer, const StrPath &fileName)
{
	auto result = FindXFile(fileName);
	if (result.first)
		return{ true, result.second };


	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	AddTask(new cTaskXFileLoader(0, &renderer, fileName));

	{
		AutoCSLock cs(m_cs);
		m_xfiles[fileName.GetHashCode()] = NULL;
		return{ true, NULL };
	}


error:
	dbg::ErrLog("Err LoadXFileParallel %s \n", fileName.c_str());
	return{ false, NULL };
}


void cResourceManager::InsertXFileModel(const StrPath &fileName, cXFileMesh *p)
{
	AutoCSLock cs(m_cs);
	m_xfiles[fileName.GetHashCode()] = p;
}


// Load Collada Model
cColladaModel* cResourceManager::LoadColladaModel( cRenderer &renderer
	, const StrPath &fileName
)
{
	auto result = FindColladaModel(fileName);
	if (result.first)
		return result.second;

	cColladaModel *model = NULL;
	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	{
		AutoCSLock cs(m_cs);
		model = new cColladaModel;
		if (!model->Create(renderer, resourcePath))
			goto error;
		
		m_colladaModels[fileName.GetHashCode()] = model;
		return model;
	}


error:
	dbg::ErrLog("Err LoadColladaModel %s \n", fileName.c_str());
	SAFE_DELETE(model);
	return NULL;
}


// Load Parallel Collada file
std::pair<bool, cColladaModel*> cResourceManager::LoadColladaModelParallel(cRenderer &renderer, const StrPath &fileName)
{
	auto result = FindColladaModel(fileName);
	if (result.first)
		return{ true, result.second };

	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	AddTask( new cTaskColladaLoader(0, &renderer, fileName) );

	{
		AutoCSLock cs(m_cs);
		m_colladaModels[fileName.GetHashCode()] = NULL;
		return{ true, NULL };
	}


error:
	dbg::ErrLog("Err LoadColladaModelParallel %s \n", fileName.c_str());
	return{ false, NULL };
}


// Set Model Pointer If Finish Parallel Load 
void cResourceManager::InsertColladaModel(const StrPath &fileName, cColladaModel *p)
{
	AutoCSLock cs(m_cs);
	m_colladaModels[fileName.GetHashCode()] = p;
}


cShadowVolume* cResourceManager::LoadShadow(cRenderer &renderer, const StrPath &fileName)
{
	auto result = FindShadow(fileName);
	if (result.first)
		return result.second;

	cShadowVolume *shadow = NULL;
	cColladaModel *collada = FindColladaModel(fileName).second;
	cXFileMesh *xfile = FindXFile(fileName).second;
	if (!collada && !xfile)
		return NULL;
	if (collada && xfile)
	{
		assert(0);
		return NULL;
	}

	if (collada)
	{
		shadow = new cShadowVolume();
		if (!collada->m_meshes.empty())
		{
			cMesh2 *mesh = collada->m_meshes[0];
			if (!shadow->Create(renderer, mesh->m_buffers->m_vtxBuff, mesh->m_buffers->m_idxBuff))
			{
				SAFE_DELETE(shadow);
			}
		}
	}
	else if (xfile)
	{
		shadow = new cShadowVolume();
		if (!shadow->Create(renderer, xfile->m_mesh, false))
		{
			SAFE_DELETE(shadow);
		}
	}

	AutoCSLock cs(m_csShadow);
	m_shadows[fileName.GetHashCode()] = shadow;
	return shadow;
}


std::pair<bool, cShadowVolume*> cResourceManager::LoadShadowParallel(cRenderer &renderer
	, const StrPath &fileName
)
{
	auto result = FindShadow(fileName);
	if (result.first)
		return{ true, result.second };

	cColladaModel *collada = FindColladaModel(fileName).second;
	cXFileMesh *xfile = FindXFile(fileName).second;
	if (!collada && !xfile)
		return{ false, NULL };
	if (collada && xfile)
	{
		assert(0);
		return{ false, NULL };
	}

	{
		AutoCSLock cs(m_csShadow);
		m_shadows[fileName.GetHashCode()] = NULL;
	}

	AddTask(new cTaskShadowLoader(0, &renderer, fileName, collada, xfile));
	
	return{ true, NULL };
}


void cResourceManager::InsertShadow(const StrPath &fileName, cShadowVolume *p)
{
	AutoCSLock cs(m_csShadow);
	m_shadows[fileName.GetHashCode()] = p;
}


// 애니메이션 파일 로딩.
sRawAniGroup* cResourceManager::LoadAnimation( const StrId &fileName )
{
	RETV(fileName.empty(), NULL);

	if (sRawAniGroup *data = FindAnimation(fileName))
		return data;

	sRawAniGroup *anies = NULL;
	const StrPath resourcePath = GetResourceFilePath(fileName.c_str());
	if (resourcePath.empty())
		goto error;

	anies = new sRawAniGroup;
	anies->name = fileName.c_str();

	if (!importer::ReadRawAnimationFile(fileName.c_str(), *anies))
		goto error;

	LoadAnimation(anies);
	return anies;


error:
	dbg::ErrLog("Err LoadAnimation %s \n", fileName.c_str());
	SAFE_DELETE(anies);
	return NULL;
}


// Register Animation Information
bool cResourceManager::LoadAnimation(sRawAniGroup *anies)
{
	RETV(!anies, false);

	m_anies[anies->name.GetHashCode()] = anies;
	return true;
}


// meshName에 해당하는 메쉬버퍼를 리턴한다.
cMeshBuffer* cResourceManager::LoadMeshBuffer(cRenderer &renderer, const StrId &meshName)
{
	if (cMeshBuffer *data = FindMeshBuffer(meshName))
		return data;

	//string fileName = meshName.c_str();
	//fileName.erase(meshName.find("::"));
	//fileName.erase(fileName.find("::"));
	StrId fileName = meshName;
	fileName.erase("::");

	if (sRawMeshGroup *meshes = LoadRawMesh(fileName.c_str()))
	{
		for each (auto &rawMesh in meshes->meshes)
		{
			if (meshName == rawMesh.name)
			{
				cMeshBuffer *buffer = new cMeshBuffer(renderer, rawMesh);
				m_mesheBuffers[ meshName.GetHashCode()] = buffer;
				return buffer;
			}
		}
	}
	
	return NULL;
}


// rawMesh 정보로 MeshBuffer를 생성한다.
cMeshBuffer* cResourceManager::LoadMeshBuffer(cRenderer &renderer, const sRawMesh &rawMesh)
{
	if (cMeshBuffer *data = FindMeshBuffer(rawMesh.name))
		return data;

	cMeshBuffer *buffer = new cMeshBuffer(renderer, rawMesh);
	m_mesheBuffers[rawMesh.name.GetHashCode()] = buffer;
	return buffer;
}


// meshName으로 메쉬버퍼를 찾아 리턴한다.
cMeshBuffer* cResourceManager::FindMeshBuffer( const StrId &meshName )
{
	auto it = m_mesheBuffers.find(meshName.GetHashCode());
	if (m_mesheBuffers.end() == it)
		return NULL; // not exist

	return it->second;
}


// find model data
sRawMeshGroup* cResourceManager::FindModel( const StrId &fileName )
{
	auto it = m_meshes.find(fileName.GetHashCode());
	if (m_meshes.end() == it)
		return NULL; // not exist

	return it->second;
}


// find model data
sRawMeshGroup2* cResourceManager::FindModel2(const StrId &fileName)
{
	auto it = m_meshes2.find(fileName.GetHashCode());
	if (m_meshes2.end() == it)
		return NULL; // not exist
	return it->second;
}


// 파일이 없다면 false를 리턴한다.
// 파일이 있다면 true를 리턴한다.
// 데이타는 NULL이 될 수 있다.
std::pair<bool, cXFileMesh*> cResourceManager::FindXFile(const StrPath &fileName)
{
	AutoCSLock cs(m_cs);

	auto it = m_xfiles.find(fileName.GetHashCode());
	if (m_xfiles.end() != it)
		return{ true, it->second };
	return{ false, NULL };
}


std::pair<bool, cColladaModel*> cResourceManager::FindColladaModel(const StrPath &fileName)
{
	AutoCSLock cs(m_cs);
	auto it = m_colladaModels.find(fileName.GetHashCode());
	if (m_colladaModels.end() != it)
		return{ true, it->second };
	return{ false, NULL };
}


std::pair<bool, cShadowVolume*> cResourceManager::FindShadow(const StrPath &fileName)
{
	AutoCSLock cs(m_csShadow);
	auto it = m_shadows.find(fileName.GetHashCode());
	if (m_shadows.end() != it)
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
	,  const bool isSizePow2 //=true
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
	if (!texture->Create(renderer, resourcePath, isSizePow2))
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
		return cResourceManager::LoadTexture(renderer, g_defaultTexture, isSizePow2, false);
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
	
	AddTask(new cTaskTextureLoader(0, &renderer, fileName, isSizePow2));

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
cCubeTexture* cResourceManager::LoadCubeTexture(cRenderer &renderer, const StrPath &fileName
	, const bool isSizePow2 //=true
	, const bool isRecursive //= true
)
{
	if (cCubeTexture *p = FindCubeTexture(fileName))
		return p;

	cCubeTexture *texture = NULL;
	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	texture = new cCubeTexture();
	if (!texture->Create(renderer, resourcePath))
		goto error;

	if (texture && texture->IsLoaded())
	{
		m_cubeTextures[fileName.GetHashCode()] = texture;
		return texture;
	}


error:
	dbg::ErrLog("Err LoadCubeTexture %s \n", fileName.c_str());
	SAFE_DELETE(texture);
	return NULL;
}


// 텍스쳐 로딩.
// fileName 에 해당하는 파일이 없다면, "../media/" + dirPath  경로에서 파일을 찾는다.
cTexture* cResourceManager::LoadTexture(cRenderer &renderer, const StrPath &dirPath, const StrPath &fileName
	, const bool isSizePow2 //= true
	, const bool isRecursive //= true
)	
{
	auto result = FindTexture(fileName);
	if (result.first)
		return result.second;

	StrPath key = fileName;
	cTexture *texture = NULL;
	if (common::IsFileExist(fileName))
	{
		texture = new cTexture();
		texture->Create(renderer, fileName, isSizePow2);
	}
	else
	{
		StrPath newPath;
		StrPath searchPath = m_mediaDirectory + dirPath;
		if (searchPath.empty())
			searchPath = ".";

		key = newPath;
		if (common::FindFile(fileName.GetFileName(), searchPath + "/", newPath))
		{
			if (isRecursive)
			{
				if (texture = cResourceManager::LoadTexture(renderer, newPath, isSizePow2, false))
					return texture;
			}
			else
			{
				if (common::IsFileExist(newPath))
				{
					texture = new cTexture();
					texture->Create(renderer, newPath, isSizePow2);
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
		// load error
		if (!texture)
		{  // Not Exist File
			return cResourceManager::LoadTexture(renderer, g_defaultTexture, isSizePow2, false);
		}

		// last load g_defaultTexture texture
		if (!texture->IsLoaded())
		{ // File Exist, but Load Error
			delete texture;
			return cResourceManager::LoadTexture(renderer, g_defaultTexture, isSizePow2, false);
		}
	}

	return NULL;
}


// 텍스쳐 로딩.
// fileName 에 해당하는 파일이 없다면, dirPath  경로에서 파일을 찾는다.
cTexture* cResourceManager::LoadTexture2(cRenderer &renderer, const StrPath &dirPath, const StrPath &fileName
	, const bool isSizePow2 //= true
	, const bool isRecursive //= true
)
{
	auto result = FindTexture(fileName);
	if (result.first)
		return result.second;

	cTexture *texture = NULL;
	const StrPath resourcePath = GetResourceFilePath(dirPath, fileName);
	if (resourcePath.empty())
		goto error;

	texture = new cTexture();
	if (!texture->Create(renderer, resourcePath, isSizePow2))
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
		return cResourceManager::LoadTexture(renderer, g_defaultTexture, isSizePow2, false);
	return NULL;
}


// 셰이더 로딩.
cShader* cResourceManager::LoadShader(cRenderer &renderer, const StrPath &fileName)
// isReload=false
{
	if (cShader *p = FindShader(fileName))
		return p;

	cShader *shader = NULL;
	const StrPath resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	shader = new cShader();
	if (!shader->Create(renderer, resourcePath, "TShader", false))
		goto error;

	if (shader)
		m_shaders[ StrPath(fileName.GetFileName()).GetHashCode()] = shader;
	return shader;


error:
	StrPath msg = StrPath("Error LoadShader ") + fileName + " 파일이 존재하지 않습니다.";
	MessageBoxA(NULL, msg.c_str(), "ERROR", MB_OK);
	dbg::ErrLog("%s\n", msg.c_str());
	SAFE_DELETE(shader);
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


cCubeTexture* cResourceManager::FindCubeTexture(const StrPath &fileName)
{
	auto it = m_cubeTextures.find(fileName.GetHashCode());
	if (m_cubeTextures.end() == it)
		return NULL; // not exist
	return it->second;
}


// 셰이더 찾기.
cShader* cResourceManager::FindShader( const StrPath &fileName )
{
	auto it = m_shaders.find(StrPath(fileName.GetFileName()).GetHashCode());
	if (m_shaders.end() == it)
		return NULL; // not exist
	return it->second;
}


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



// media 폴더내에 fileName이 있으면, 경로를 리턴한다.
StrPath cResourceManager::GetResourceFilePath(const StrPath &fileName)
{
	if (fileName.IsFileExist())
	{
		return fileName;
	}
	else
	{
		const StrPath composeMediaFileName = m_mediaDirectory + fileName;
		if (composeMediaFileName.IsFileExist())
		{
			return composeMediaFileName;
		}
		else
		{
			StrPath newPath;
			if (common::FindFile(fileName, m_mediaDirectory, newPath))
			{
				return newPath;
			}
		}
	}
	return "";
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

			if (common::FindFile(fileName, searchPath, newPath, 5))
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

	// remove raw mesh
	for each (auto kv in m_meshes)
	{
		delete kv.second;
	}
	m_meshes.clear();

	// remove raw mesh2
	for each (auto kv in m_meshes2)
	{
		delete kv.second;
	}
	m_meshes2.clear();

	// remove raw m_xfiles
	{
		AutoCSLock cs(m_cs);
		for each (auto kv in m_xfiles)
		{
			delete kv.second;
		}
		m_xfiles.clear();
	}

	// remove raw collada models
	{
		AutoCSLock cs(m_cs);
		for each (auto kv in m_colladaModels)
		{
			delete kv.second;
		}
		m_colladaModels.clear();
	}

	// remove shadow
	{
		AutoCSLock cs(m_csShadow);
		for each (auto kv in m_shadows)
		{
			delete kv.second;
		}
		m_shadows.clear();
	}

	// remove texture
	{
		AutoCSLock cs(m_cs);
		cTexture *defaultTex = m_textures[g_defaultTexture.GetHashCode()];
		for each (auto kv in m_textures)
		{
			if (kv.second != defaultTex)
				delete kv.second;
		}
		SAFE_DELETE(defaultTex);
		m_textures.clear();
	}

	// remove cube texture
	for each (auto kv in m_cubeTextures)
	{
		delete kv.second;
	}
	m_cubeTextures.clear();


	// remove raw ani
	for each (auto kv in m_anies)
	{
		delete kv.second;
	}
	m_anies.clear();

	// remove shader
	for each (auto kv in m_shaders)
	{
		delete kv.second;
	}
	m_shaders.clear();

	// remove mesh buffer
	for each (auto kv in m_mesheBuffers)
	{
		delete kv.second;
	}
	m_mesheBuffers.clear();
}


// 파일 종류를 리턴한다.
RESOURCE_TYPE::TYPE cResourceManager::GetFileKind( const StrPath &fileName )
{
	return importer::GetFileKind(fileName.c_str());
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


void cResourceManager::ReloadShader(cRenderer &renderer)
{
	for (auto p : m_shaders)
		p.second->Reload(renderer);
}


void cResourceManager::LostDevice()
{
	{
		AutoCSLock cs(m_cs);
		for (auto &p : m_textures)
			if (p.second)
				p.second->LostDevice();
	}

	for (auto &p : m_cubeTextures)
		p.second->LostDevice();
	for (auto &p : m_shaders)
		p.second->LostDevice();
}


void cResourceManager::ResetDevice(cRenderer &renderer)
{
	{
		AutoCSLock cs(m_cs);
		for (auto &p : m_textures)
			if (p.second)
				p.second->ResetDevice(renderer);
	}

	for (auto &p : m_cubeTextures)
		p.second->ResetDevice(renderer);
	for (auto &p : m_shaders)
		p.second->ResetDevice(renderer);
}

