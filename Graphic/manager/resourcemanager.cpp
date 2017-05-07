
#include "stdafx.h"
#include "resourcemanager.h"
#include "../importer/modelimporter.h"
#include "../base/material.h"
#include "../model_collada/model_collada.h"
#include "task_resource.h"


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


// load model file
sRawMeshGroup* cResourceManager::LoadRawMesh( const string &fileName )
{
	RETV(fileName.empty(), NULL);

	if (sRawMeshGroup *data = FindModel(fileName))
		return data;

	sRawMeshGroup *meshes = NULL;
	const string resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	meshes = new sRawMeshGroup;
	meshes->name = fileName;

	if (!importer::ReadRawMeshFile(resourcePath, *meshes))
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

	m_meshes[meshes->name] = meshes;
	return true;
}


// load collada model file
sRawMeshGroup2* cResourceManager::LoadRawMesh2(const string &fileName)
{
	RETV(fileName.empty(), NULL);

	if (sRawMeshGroup2 *data = FindModel2(fileName))
		return data;

	cColladaLoader loader;
	const string resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	if (!loader.Create(resourcePath))
		goto error;

	if (loader.m_rawMeshes)
		m_meshes2[fileName] = loader.m_rawMeshes;
	if (loader.m_rawAnies)
	{
		loader.m_rawMeshes->animationName = loader.m_rawAnies->name;
		m_anies[loader.m_rawAnies->name] = loader.m_rawAnies;
	}
	return loader.m_rawMeshes;


error:
	dbg::ErrLog("Err LoadRawMesh2 %s \n", fileName.c_str());
	return NULL;
}


cXFileMesh* cResourceManager::LoadXFile(cRenderer &renderer, const string &fileName)
{
	auto result = FindXFile(fileName);
	if (result.first)
		return result.second;

	cXFileMesh *mesh = NULL;
	const string resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	mesh = new cXFileMesh;
	if (!mesh->Create(renderer, resourcePath, false, true))
		goto error;

	m_xfiles[fileName] = mesh;
	return mesh;


error:
	dbg::ErrLog("Err LoadXFile %s \n", fileName.c_str());
	SAFE_DELETE(mesh);
	return NULL;
}


std::pair<bool, cXFileMesh*> cResourceManager::LoadXFileParallel(cRenderer &renderer, const string &fileName)
{
	auto result = FindXFile(fileName);
	if (result.first)
		return{ true, result.second };


	const string resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	m_loadThread.AddTask(new cTaskXFileLoader(++m_loadId, &renderer, fileName));
	if (!m_loadThread.IsRun())
		m_loadThread.Start();

	{
		AutoCSLock cs(m_cs);
		m_xfiles[fileName] = NULL;
		return{ true, NULL };
	}


error:
	dbg::ErrLog("Err LoadXFileParallel %s \n", fileName.c_str());
	return{ false, NULL };
}


void cResourceManager::InsertXFileModel(const string &fileName, cXFileMesh *p)
{
	AutoCSLock cs(m_cs);
	m_xfiles[fileName] = p;
}


// Load Collada Model
cColladaModel* cResourceManager::LoadColladaModel( cRenderer &renderer
	, const string &fileName
)
{
	if (cColladaModel *p = FindColladaModel(fileName))
		return p;

	cColladaModel *model = NULL;
	const string resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	{
		AutoCSLock cs(m_cs);
		model = new cColladaModel;
		if (!model->Create(renderer, resourcePath))
			goto error;
		
		m_colladaModels[fileName] = model;
		return model;
	}


error:
	dbg::ErrLog("Err LoadColladaModel %s \n", fileName.c_str());
	SAFE_DELETE(model);
	return NULL;
}


// Load Parallel Collada file
std::pair<bool, cColladaModel*> cResourceManager::LoadColladaModelParallel(cRenderer &renderer, const string &fileName)
{
	if (cColladaModel *p = FindColladaModel(fileName))
		return{ true, p };

	const string resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	m_loadThread.AddTask( new cTaskColladaLoader(++m_loadId, &renderer, fileName) );
	if (!m_loadThread.IsRun())
		m_loadThread.Start();

	{
		AutoCSLock cs(m_cs);
		m_colladaModels[fileName] = NULL;
		return{ true, NULL };
	}


error:
	dbg::ErrLog("Err LoadColladaModelParallel %s \n", fileName.c_str());
	return{ false, NULL };
}


// Set Model Pointer If Finish Parallel Load 
void cResourceManager::InsertColladaModel(const string &fileName, cColladaModel *p)
{
	AutoCSLock cs(m_cs);
	m_colladaModels[fileName] = p;
}


cShadowVolume* cResourceManager::LoadShadow(cRenderer &renderer, const string &fileName)
{
	auto result = FindShadow(fileName);
	if (result.first)
		return result.second;

	cShadowVolume *shadow = NULL;
	cColladaModel *collada = FindColladaModel(fileName);
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
	m_shadows[fileName] = shadow;
	return shadow;
}


std::pair<bool, cShadowVolume*> cResourceManager::LoadShadowParallel(cRenderer &renderer
	, const string &fileName
)
{
	auto result = FindShadow(fileName);
	if (result.first)
		return{ true, result.second };

	cColladaModel *collada = FindColladaModel(fileName);
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
		m_shadows[fileName] = NULL;
	}

	m_loadThread.AddTask(new cTaskShadowLoader(++m_loadId, &renderer, fileName, collada, xfile));
	if (!m_loadThread.IsRun())
		m_loadThread.Start();

	return{ true, NULL };
}


void cResourceManager::InsertShadow(const string &fileName, cShadowVolume *p)
{
	AutoCSLock cs(m_csShadow);
	m_shadows[fileName] = p;
}


// 애니메이션 파일 로딩.
sRawAniGroup* cResourceManager::LoadAnimation( const string &fileName )
{
	RETV(fileName.empty(), NULL);

	if (sRawAniGroup *data = FindAnimation(fileName))
		return data;

	sRawAniGroup *anies = NULL;
	const string resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	anies = new sRawAniGroup;
	anies->name = fileName;

	if (!importer::ReadRawAnimationFile(fileName, *anies))
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

	m_anies[anies->name] = anies;
	return true;
}


// meshName에 해당하는 메쉬버퍼를 리턴한다.
cMeshBuffer* cResourceManager::LoadMeshBuffer(cRenderer &renderer, const string &meshName)
{
	if (cMeshBuffer *data = FindMeshBuffer(meshName))
		return data;

	string fileName = meshName;
	fileName.erase(meshName.find("::"));

	if (sRawMeshGroup *meshes = LoadRawMesh(fileName))
	{
		for each (auto &rawMesh in meshes->meshes)
		{
			if (meshName == rawMesh.name)
			{
				cMeshBuffer *buffer = new cMeshBuffer(renderer, rawMesh);
				m_mesheBuffers[ meshName] = buffer;
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
	m_mesheBuffers[rawMesh.name] = buffer;
	return buffer;
}


// meshName으로 메쉬버퍼를 찾아 리턴한다.
cMeshBuffer* cResourceManager::FindMeshBuffer( const string &meshName )
{
	auto it = m_mesheBuffers.find(meshName);
	if (m_mesheBuffers.end() == it)
		return NULL; // not exist

	return it->second;
}


// find model data
sRawMeshGroup* cResourceManager::FindModel( const string &fileName )
{
	auto it = m_meshes.find(fileName);
	if (m_meshes.end() == it)
		return NULL; // not exist

	return it->second;
}


// find model data
sRawMeshGroup2* cResourceManager::FindModel2(const string &fileName)
{
	auto it = m_meshes2.find(fileName);
	if (m_meshes2.end() == it)
		return NULL; // not exist
	return it->second;
}


// 파일이 없다면 false를 리턴한다.
// 파일이 있다면 true를 리턴한다.
// 데이타는 NULL이 될 수 있다.
std::pair<bool, cXFileMesh*> cResourceManager::FindXFile(const string &fileName)
{
	AutoCSLock cs(m_cs);

	auto it = m_xfiles.find(fileName);
	if (m_xfiles.end() != it)
		return{ true, it->second };
	return{ false, NULL };
}


cColladaModel * cResourceManager::FindColladaModel(const string &fileName)
{
	AutoCSLock cs(m_cs);
	auto it = m_colladaModels.find(fileName);
	if (m_colladaModels.end() != it)
		return it->second;
	return NULL;
}


std::pair<bool, cShadowVolume*> cResourceManager::FindShadow(const string &fileName)
{
	AutoCSLock cs(m_csShadow);
	auto it = m_shadows.find(fileName);
	if (m_shadows.end() != it)
		return{ true, it->second };
	return{ false, NULL };
}


// find animation data
sRawAniGroup* cResourceManager::FindAnimation( const string &fileName )
{
	auto it = m_anies.find(fileName);
	if (m_anies.end() == it)
		return NULL; // not exist
	return it->second;
}


// 텍스쳐 로딩.
cTexture* cResourceManager::LoadTexture(cRenderer &renderer, const string &fileName
	,  const bool isSizePow2 //=true
	, const bool isRecursive //= true
)
{
	if (cTexture *p = FindTexture(fileName))
		return p;

	cTexture *texture = NULL;
	const string resourcePath = GetResourceFilePath(fileName);
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
		m_textures[fileName] = texture;
		return texture;
	}


error:
	dbg::ErrLog("Error LoadTexture1 %s \n", fileName.c_str());
	SAFE_DELETE(texture);
	if (isRecursive)
		return cResourceManager::LoadTexture(renderer, g_defaultTexture, isSizePow2, false);
	return NULL;
}


// 텍스쳐 로딩.
cCubeTexture* cResourceManager::LoadCubeTexture(cRenderer &renderer, const string &fileName
	, const bool isSizePow2 //=true
	, const bool isRecursive //= true
)
{
	if (cCubeTexture *p = FindCubeTexture(fileName))
		return p;

	cCubeTexture *texture = NULL;
	const string resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	texture = new cCubeTexture();
	if (!texture->Create(renderer, resourcePath))
		goto error;

	if (texture && texture->IsLoaded())
	{
		m_cubeTextures[fileName] = texture;
		return texture;
	}


error:
	dbg::ErrLog("Err LoadCubeTexture %s \n", fileName.c_str());
	SAFE_DELETE(texture);
	return NULL;
}


// 텍스쳐 로딩.
// fileName 에 해당하는 파일이 없다면, "../media/" + dirPath  경로에서 파일을 찾는다.
cTexture* cResourceManager::LoadTexture(cRenderer &renderer, const string &dirPath, const string &fileName
	, const bool isSizePow2 //= true
	, const bool isRecursive //= true
)	
{
	if (cTexture *p = FindTexture(fileName))
		return p;

	string key = fileName;
	cTexture *texture = NULL;// new cTexture();
	if (common::IsFileExist(fileName))
	{
		texture = new cTexture();
		texture->Create(renderer, fileName, isSizePow2);
	}
	else
	{
		string newPath;
		string searchPath = m_mediaDirectory + dirPath;
		if (searchPath.empty())
			searchPath = ".";

		key = newPath;
		if (common::FindFile(GetFileName(fileName), searchPath + "/", newPath))
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
		m_textures[key] = texture;
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
cTexture* cResourceManager::LoadTexture2(cRenderer &renderer, const string &dirPath, const string &fileName
	, const bool isSizePow2 //= true
	, const bool isRecursive //= true
)
{
	if (cTexture *p = FindTexture(fileName))
		return p;

	cTexture *texture = NULL;
	const string resourcePath = GetResourceFilePath(dirPath, fileName);
	if (resourcePath.empty())
		goto error;

	texture = new cTexture();
	if (!texture->Create(renderer, resourcePath, isSizePow2))
		goto error;

	if (texture)
		m_textures[fileName] = texture;
	return texture;


error:
	dbg::ErrLog("Error LoadTexture2 %s \n", fileName.c_str());
	SAFE_DELETE(texture);
	if (isRecursive)
		return cResourceManager::LoadTexture(renderer, g_defaultTexture, isSizePow2, false);
	return NULL;
}


// 셰이더 로딩.
cShader* cResourceManager::LoadShader(cRenderer &renderer, const string &fileName)
// isReload=false
{
	if (cShader *p = FindShader(fileName))
		return p;

	cShader *shader = NULL;
	const string resourcePath = GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	shader = new cShader();
	if (!shader->Create(renderer, resourcePath, "TShader", false))
		goto error;

	if (shader)
		m_shaders[ GetFileName(fileName)] = shader;
	return shader;


error:
	string msg = string("Error LoadShader ") + fileName + " 파일이 존재하지 않습니다.";
	MessageBoxA(NULL, msg.c_str(), "ERROR", MB_OK);
	dbg::ErrLog("%s\n", msg.c_str());
	SAFE_DELETE(shader);
	return NULL;
}


// 텍스쳐 찾기.
cTexture* cResourceManager::FindTexture( const string &fileName )
{
	auto it = m_textures.find(fileName);
	if (m_textures.end() == it)
		return NULL; // not exist
	return it->second;
}


cCubeTexture* cResourceManager::FindCubeTexture(const string &fileName)
{
	auto it = m_cubeTextures.find(fileName);
	if (m_cubeTextures.end() == it)
		return NULL; // not exist
	return it->second;
}


// 셰이더 찾기.
cShader* cResourceManager::FindShader( const string &fileName )
{
	auto it = m_shaders.find(GetFileName(fileName));
	if (m_shaders.end() == it)
		return NULL; // not exist
	return it->second;
}


// media 폴더내에 fileName 에 해당하는 파일이 존재한다면,
// 전체 경로를 리턴한다. 파일을 찾을 때는 파일이름만 비교한다.
string cResourceManager::FindFile( const string &fileName )
{
	string newPath;
	if (common::FindFile(fileName, m_mediaDirectory, newPath))
	{
		return newPath;
	}
	return ""; //empty string
}


// media 폴더내에 fileName이 있으면, 경로를 리턴한다.
string cResourceManager::GetResourceFilePath(const string &fileName)
{
	if (common::IsFileExist(fileName))
	{
		return fileName;
	}
	else
	{
		const string composeMediaFileName = m_mediaDirectory + fileName;
		if (common::IsFileExist(composeMediaFileName))
		{
			return composeMediaFileName;
		}
		else
		{
			string newPath;
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
string cResourceManager::GetResourceFilePath(const string &dir, const string &fileName)
{
	if (common::IsFileExist(fileName))
	{
		return fileName;
	}
	else
	{
		const string composeMediaFileName = dir + fileName;
		if (common::IsFileExist(composeMediaFileName))
		{
			return composeMediaFileName;
		}
		else
		{
			string newPath;
			string searchPath = dir;
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
	for each (auto kv in m_textures)
	{
		delete kv.second;
	}
	m_textures.clear();

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
RESOURCE_TYPE::TYPE cResourceManager::GetFileKind( const string &fileName )
{
	return importer::GetFileKind(fileName);
}


// 파일경로 fileName을 media 폴더의 상대 주소로 바꿔서 리턴한다.
// ex)
// media : c:/project/media,  
// fileName : c:/project/media/terrain/file.txt
// result = ./terrain/file.txt
string cResourceManager::GetRelativePathToMedia( const string &fileName )
{
	const string mediaFullPath = common::GetFullFileName(m_mediaDirectory);
	const string fullFileName = common::GetFullFileName(fileName);
	const string relatePath = common::RelativePathTo( mediaFullPath, fullFileName);
	return relatePath;
}


void cResourceManager::ReloadShader(cRenderer &renderer)
{
	for (auto p : m_shaders)
		p.second->Reload(renderer);
}


void cResourceManager::LostDevice()
{
	for (auto &p : m_textures)
		p.second->LostDevice();
	for (auto &p : m_cubeTextures)
		p.second->LostDevice();
	for (auto &p : m_shaders)
		p.second->LostDevice();
}


void cResourceManager::ResetDevice(cRenderer &renderer)
{
	for (auto &p : m_textures)
		p.second->ResetDevice(renderer);
	for (auto &p : m_cubeTextures)
		p.second->ResetDevice(renderer);
	for (auto &p : m_shaders)
		p.second->ResetDevice(renderer);
}

