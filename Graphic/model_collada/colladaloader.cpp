
#include "stdafx.h"
#include "colladaloader.h"


using namespace graphic;
using namespace Assimp;

cColladaLoader::cColladaLoader()
	: m_rawMeshes(NULL)
	, m_rawAnies(NULL)
	, m_aiScene(NULL)
{
}

cColladaLoader::~cColladaLoader()
{
}


bool cColladaLoader::Create(const StrPath &fileName)
{
	m_rawMeshes = new sRawMeshGroup2;
	m_rawMeshes->name = fileName;

	// Create the importer
	Assimp::Importer importer;

	m_aiScene = importer.ReadFile(fileName.c_str(),
		aiProcess_JoinIdenticalVertices |		// join identical vertices/ optimize indexing
		aiProcess_ValidateDataStructure |		// perform a full validation of the loader's output
		aiProcess_ImproveCacheLocality |		// improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials |	// remove redundant materials
		aiProcess_GenUVCoords |					// convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords |			// pre-process UV transformations (scaling, translation ...)
		aiProcess_FindInstances |				// search for instanced meshes and remove them by references to one master
		aiProcess_LimitBoneWeights |			// limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes |				// join small meshes, if possible;
		aiProcess_GenSmoothNormals |			// generate smooth normal vectors if not existing
		aiProcess_SplitLargeMeshes |			// split large, unrenderable meshes into sub-meshes
		aiProcess_Triangulate |					// triangulate polygons with more than 3 edges
		aiProcess_ConvertToLeftHanded |			// convert everything to D3D left handed space
		aiProcess_SortByPType);					// make 'clean' meshes which consist of a single type of primitives

	// If the import failed, report it
	if (!m_aiScene)
	{
		return false;
	}

	m_fileName = fileName;
	m_numMeshes = m_aiScene->mNumMeshes;
	m_numMaterials = m_aiScene->mNumMaterials;
	m_numTextures = m_aiScene->mNumTextures;
	m_numAnimations = m_aiScene->mNumAnimations;
	m_hasAnimations = m_numAnimations > 0;

	FindBoneNode();
	CreateSimpleBones(m_aiScene->mRootNode, -1, m_aiBones, m_fullHierarchy);
	MarkParents(m_fullHierarchy);
	FilterHierarchy(m_fullHierarchy, m_reducedHierarchy);

	CreateMesh();
	CreateBone();
	CreateNode(m_aiScene->mRootNode);
	CreateMeshBone(m_aiScene->mRootNode);
	CreateAnimation();

	return true;
}


// find all bones that influence the meshes first
void cColladaLoader::FindBoneNode()
{
	for (unsigned int m = 0; m < m_aiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_aiScene->mMeshes[m];
		assert(mesh);

		for (unsigned int b = 0; b < mesh->mNumBones; ++b)
		{
			aiBone* bone = mesh->mBones[b];
			assert(bone);
			m_aiBones[bone->mName.data] = bone;
		}
	}
}


void cColladaLoader::CreateSimpleBones(const aiNode* node, int parent,
	const map<string, aiBone*>& animatedNodes, vector<SkeletonNode>& result) const
{
	if (!node)
		return;

	string nodeName = string(node->mName.data);
	map<string, aiBone*>::const_iterator itBone = animatedNodes.find(nodeName);
	bool isAnimated = itBone != animatedNodes.end();

	SkeletonNode _n =
	{
		node,
		isAnimated ? itBone->second : NULL,
		parent,
		nodeName,
		isAnimated
	};

	result.push_back(_n);
	parent = result.size() - 1;

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		CreateSimpleBones(node->mChildren[i], parent, animatedNodes, result);
}



void  cColladaLoader::MarkParents(std::vector<SkeletonNode>& hierarchy) const
{
	for (unsigned int i = 0; i < hierarchy.size(); ++i)
	{
		SkeletonNode& currentNode = hierarchy[i];
		if (currentNode.used)
		{
			char p = currentNode.parent;
			while (p >= 0)
			{
				SkeletonNode& n = hierarchy[p];
				if (n.used) break;

				n.used = true;
				p = n.parent;
			}
		}
	}
}


void cColladaLoader::FilterHierarchy(const std::vector<SkeletonNode>& fullHierarchy, std::vector<SkeletonNode>& result) const
{
	map<int, int> nodeMapping;
	nodeMapping[-1] = -1;

	for (unsigned int i = 0; i < fullHierarchy.size(); ++i)
	{
		SkeletonNode n = fullHierarchy[i];
		if (n.used)
		{
			n.parent = nodeMapping[n.parent];
			result.push_back(n);

			nodeMapping[i] = result.size() - 1;
		}
	}
}


void cColladaLoader::CreateMesh()
{
	for (unsigned int m = 0; m < m_aiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_aiScene->mMeshes[m];
		m_rawMeshes->meshes.push_back(sRawMesh2());
		sRawMesh2 *rawMesh = &m_rawMeshes->meshes.back();

		rawMesh->name = format("mesh%d", m);
		CreateMaterial(mesh, rawMesh->mtrl);
		rawMesh->mtrl.ambient = Vector4(1, 1, 1, 1) * 0.4f;

		int NumBones = mesh->mNumBones;
		int FaceCount = mesh->mNumFaces;
		int VertexCount = mesh->mNumVertices;

		unsigned int nidx;
		switch (mesh->mPrimitiveTypes) {
		case aiPrimitiveType_POINT:
			nidx = 1; break;
		case aiPrimitiveType_LINE:
			nidx = 2; break;
		case aiPrimitiveType_TRIANGLE:
			nidx = 3; break;
		default:
			nidx = 3;
		};

		// check whether we can use 16 bit indices
		bool SaveIndicesAs16Bit = true;
		if (mesh->mNumFaces * nidx >= 65536)
		{
			SaveIndicesAs16Bit = false;
		}

		// now fill the index buffer
		rawMesh->indices.reserve(mesh->mNumFaces * 3);

		for (unsigned int x = 0; x < mesh->mNumFaces; ++x)
		{
			for (unsigned int a = 0; a < nidx; ++a)
			{
				rawMesh->indices.push_back(mesh->mFaces[x].mIndices[a]);
			}
		}

		rawMesh->vertices.reserve(mesh->mNumVertices);
		rawMesh->normals.reserve(mesh->mNumVertices);
		rawMesh->tex.reserve(mesh->mNumVertices);
		rawMesh->weights.reserve(mesh->mNumVertices);

		// collect weights on all vertices
		vector<vector<aiVertexWeight> > weightsPerVertex(mesh->mNumVertices);
		for (unsigned int a = 0; a < mesh->mNumBones; a++)
		{
			const aiBone* bone = mesh->mBones[a];
			for (unsigned int b = 0; b < bone->mNumWeights; b++)
			{
				weightsPerVertex[bone->mWeights[b].mVertexId].push_back(
					aiVertexWeight(a, bone->mWeights[b].mWeight));
			}
		}

		for (unsigned int x = 0; x < mesh->mNumVertices; ++x)
		{
			Vector3 position;
			Vector3 normal(0.0f, 0.0f, 0.0f);
			Vector3 texture(0.5f, 0.5f, 0.5f);
			unsigned char boneIndices[4] = { 0, 0, 0, 0 };
			float boneWeights[4] = { 0, 0, 0, 0 };

			position.x = mesh->mVertices[x].x;
			position.y = mesh->mVertices[x].y;
			position.z = mesh->mVertices[x].z;

			if (NULL != mesh->mNormals)
			{
				normal.x = mesh->mNormals[x].x;
				normal.y = mesh->mNormals[x].y;
				normal.z = mesh->mNormals[x].z;
			}

			if (mesh->HasTextureCoords(0))
			{
				texture.x = mesh->mTextureCoords[0][x].x;
				texture.y = mesh->mTextureCoords[0][x].y;
			}

			if (mesh->HasBones())
			{
				assert(weightsPerVertex[x].size() <= 4);
				sVertexWeight weight;
				weight.vtxIdx = x;
				weight.size = weightsPerVertex[x].size();
				for (unsigned int a = 0; a < weightsPerVertex[x].size(); a++)
				{
					sWeight w;
					w.bone = weightsPerVertex[x][a].mVertexId;
					w.weight = weightsPerVertex[x][a].mWeight;
					weight.w[a] = w;
				}

				rawMesh->weights.push_back(weight);
			}

			rawMesh->vertices.push_back(position);
			rawMesh->normals.push_back(normal);
			rawMesh->tex.push_back(texture);
		}
	}
}


void cColladaLoader::CreateMaterial(const aiMesh *sourceMesh, OUT sMaterial &mtrl)
{
	// extract all properties from the ASSIMP material structure
	const aiMaterial* sourceMaterial = m_aiScene->mMaterials[sourceMesh->mMaterialIndex];

	//
	// DIFFUSE COLOR
	//
	aiColor4D materialColor;
	if (AI_SUCCESS == aiGetMaterialColor(sourceMaterial, AI_MATKEY_COLOR_DIFFUSE, &materialColor))
	{
		mtrl.diffuse.x = materialColor.r;
		mtrl.diffuse.y = materialColor.g;
		mtrl.diffuse.z = materialColor.b;
		mtrl.diffuse.w = materialColor.a;
	}
	else
	{
		mtrl.diffuse.x = 1.0f;
		mtrl.diffuse.y = 1.0f;
		mtrl.diffuse.z = 1.0f;
		mtrl.diffuse.w = 1.0f;
	}
	//
	// SPECULAR COLOR
	//
	if (AI_SUCCESS == aiGetMaterialColor(sourceMaterial, AI_MATKEY_COLOR_SPECULAR, &materialColor))
	{
		mtrl.specular.x = materialColor.r;
		mtrl.specular.y = materialColor.g;
		mtrl.specular.z = materialColor.b;
		mtrl.specular.w = materialColor.a;
	}
	else
	{
		mtrl.specular.x = 1.0f;
		mtrl.specular.y = 1.0f;
		mtrl.specular.z = 1.0f;
		mtrl.specular.w = 1.0f;
	}

	//
	// AMBIENT COLOR
	//
	if (AI_SUCCESS == aiGetMaterialColor(sourceMaterial, AI_MATKEY_COLOR_AMBIENT, &materialColor))
	{
		mtrl.ambient.x = materialColor.r;
		mtrl.ambient.y = materialColor.g;
		mtrl.ambient.z = materialColor.b;
		mtrl.ambient.w = materialColor.a;
	}
	else
	{
		mtrl.ambient.x = 1.0f;
		mtrl.ambient.y = 1.0f;
		mtrl.ambient.z = 1.0f;
		mtrl.ambient.w = 1.0f;
	}

	//
	// EMISSIVE COLOR
	//
	if (AI_SUCCESS == aiGetMaterialColor(sourceMaterial, AI_MATKEY_COLOR_EMISSIVE, &materialColor))
	{
		mtrl.emissive.x = materialColor.r;
		mtrl.emissive.y = materialColor.g;
		mtrl.emissive.z = materialColor.b;
		mtrl.emissive.w = materialColor.a;
	}
	else
	{
		mtrl.emissive.x = 1.0f;
		mtrl.emissive.y = 1.0f;
		mtrl.emissive.z = 1.0f;
		mtrl.emissive.w = 1.0f;
	}

	//
	// Opacity
	//
	//if (AI_SUCCESS != aiGetMaterialFloat(sourceMaterial, AI_MATKEY_OPACITY, &mtrl->MaterialOpacity))
	//{
		//mtrl->MaterialOpacity = 1.0f;
	//}

	//
	// Shininess
	//
	//if (AI_SUCCESS != aiGetMaterialFloat(sourceMaterial, AI_MATKEY_SHININESS, &mtrl->MaterialShininess))
	//{
		// assume 15 as default shininess
		//mtrl->MaterialShininess = 15.0f;
	//}

	//
	// Shininess strength
	//
	if (AI_SUCCESS != aiGetMaterialFloat(sourceMaterial, AI_MATKEY_SHININESS_STRENGTH, &mtrl.power))
	{
		// assume 1.0 as default shininess strength
		mtrl.power = 1.0f;
	}

	aiString szPath;
	aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);

	//bool bib = false;
	if (sourceMesh->mTextureCoords[0])
	{
		// Diffuse texture
		if (AI_SUCCESS == aiGetMaterialString(sourceMaterial, AI_MATKEY_TEXTURE_DIFFUSE(0), &szPath))
		{
			string fileName = szPath.data;
			mtrl.texture = fileName;
		}

		// Normal texture
		if (AI_SUCCESS == aiGetMaterialString(sourceMaterial, AI_MATKEY_TEXTURE_NORMALS(0), &szPath))
		{
			//string fileName = szPath.data;
			//mtrl.normalmap = fileName;
		}
	}
}


int cColladaLoader::GetBoneId(const string &boneName)
{
	for (u_int i = 0; i < m_reducedHierarchy.size(); ++i)
	{
		if (m_reducedHierarchy[i].name == boneName)
			return i;
	}
	return -1;
}


void cColladaLoader::CreateBone()
{
	m_rawMeshes->bones.resize(m_reducedHierarchy.size());

	int i = 0;
	for (auto bone : m_reducedHierarchy)
	{
		sRawBone2 b;
		b.id = GetBoneId(bone.name);
		b.parentId = bone.parent;
		b.name = bone.name;

		if (bone.bone)
		{
			b.localTm = *(Matrix44*)&bone.node->mTransformation;
			b.localTm.Transpose();

			b.offsetTm = *(Matrix44*)&bone.bone->mOffsetMatrix;
			b.offsetTm.Transpose();
		}

		m_rawMeshes->bones[i++] = b;
	}
}


void cColladaLoader::CreateAnimation()
{
	RET(m_aiScene->mNumAnimations <= 0);

	for (size_t j = 0; j < 1; j++)
	{
		const aiAnimation* sourceAnimation = m_aiScene->mAnimations[j];

		//animation.TicksPerSecond = static_cast<float>(sourceAnimation->mTicksPerSecond);
		//animation.Duration = static_cast<float>(sourceAnimation->mDuration);

		m_rawAnies = new sRawAniGroup;
		m_rawAnies->type = sRawAniGroup::BONE_ANI;
		m_rawAnies->name = string(m_fileName.GetFileName()) + "::" + sourceAnimation->mName.data;
		m_rawAnies->anies.resize(m_rawMeshes->bones.size());

		for (UINT a = 0; a < sourceAnimation->mNumChannels; a++)
		{
			string boneName = sourceAnimation->mChannels[a]->mNodeName.data;
			const int boneId = GetBoneId(boneName);
			if (boneId < 0)
				continue;

			sRawAni &ani = m_rawAnies->anies[boneId];
			ani.pos.resize(sourceAnimation->mChannels[a]->mNumPositionKeys);
			ani.rot.resize(sourceAnimation->mChannels[a]->mNumPositionKeys);
			ani.scale.resize(sourceAnimation->mChannels[a]->mNumPositionKeys);

			for (unsigned int i = 0; i < sourceAnimation->mChannels[a]->mNumPositionKeys; i++)
			{
				float timePos = static_cast<float>(sourceAnimation->mChannels[a]->mPositionKeys[i].mTime);

				if (i == 0)
					ani.start = timePos;
				else if (i == (sourceAnimation->mChannels[a]->mNumPositionKeys - 1))
					ani.end = timePos;

				ani.pos[i].t = timePos;
				ani.pos[i].p.x = sourceAnimation->mChannels[a]->mPositionKeys[i].mValue.x;
				ani.pos[i].p.y = sourceAnimation->mChannels[a]->mPositionKeys[i].mValue.y;
				ani.pos[i].p.z = sourceAnimation->mChannels[a]->mPositionKeys[i].mValue.z;

				ani.rot[i].t = timePos;
				ani.rot[i].q.x = sourceAnimation->mChannels[a]->mRotationKeys[i].mValue.x;
				ani.rot[i].q.y = sourceAnimation->mChannels[a]->mRotationKeys[i].mValue.y;
				ani.rot[i].q.z = sourceAnimation->mChannels[a]->mRotationKeys[i].mValue.z;
				ani.rot[i].q.w = sourceAnimation->mChannels[a]->mRotationKeys[i].mValue.w;

				ani.scale[i].t = timePos;
				ani.scale[i].s.x = sourceAnimation->mChannels[a]->mScalingKeys[i].mValue.x;
				ani.scale[i].s.y = sourceAnimation->mChannels[a]->mScalingKeys[i].mValue.y;
				ani.scale[i].s.z = sourceAnimation->mChannels[a]->mScalingKeys[i].mValue.z;
			}
		}
	}
}


void cColladaLoader::CreateNode(aiNode* node)
{
	for (u_int m = 0; m < node->mNumMeshes; ++m)
	{
		sRawMesh2 &mesh = m_rawMeshes->meshes[node->mMeshes[m]];
		mesh.localTm = *(Matrix44*)&node->mTransformation;
		mesh.localTm.Transpose();
	}

	for (u_int c = 0; c < node->mNumChildren; c++)
		CreateNode(node->mChildren[c]);
}


void cColladaLoader::CreateMeshBone(aiNode* node)
{
	for (u_int m = 0; m < node->mNumMeshes; ++m)
	{
		aiMesh *mesh = m_aiScene->mMeshes[node->mMeshes[m]];
		sRawMesh2 &rawMesh = m_rawMeshes->meshes[node->mMeshes[m]];
		rawMesh.localTm = *(Matrix44*)&node->mTransformation;
		rawMesh.localTm.Transpose();

		rawMesh.bones.reserve(4);
		for (auto b : rawMesh.bones)
			b.id = 0;

		for (unsigned int a = 0; a < mesh->mNumBones; a++)
		{
			const aiBone* bone = mesh->mBones[a];
			const int boneId = GetBoneId(bone->mName.data);
			assert(boneId >= 0);
			if (boneId < 0)
				continue;

			rawMesh.bones.push_back(sMeshBone());
			sMeshBone *b = &rawMesh.bones.back();
			b->id = boneId;
			b->name = bone->mName.data;
			b->offsetTm = *(Matrix44*)&bone->mOffsetMatrix;
			b->offsetTm.Transpose();
		}
	}

	for (u_int c = 0; c < node->mNumChildren; c++)
		CreateMeshBone(node->mChildren[c]);
}

