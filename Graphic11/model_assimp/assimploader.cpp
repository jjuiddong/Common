
#include "stdafx.h"
#include "assimploader.h"


using namespace graphic;
using namespace Assimp;

cAssimpLoader::cAssimpLoader()
	: m_rawMeshes(NULL)
	, m_rawAnies(NULL)
	, m_aiScene(NULL)
{
}

cAssimpLoader::~cAssimpLoader()
{
}


bool cAssimpLoader::Create(const StrPath &fileName)
{
	SAFE_DELETE(m_rawMeshes);
	m_rawMeshes = new sRawMeshGroup2;
	m_rawMeshes->name = fileName.c_str();

	// Create the importer
	Assimp::Importer importer;

	m_aiScene = importer.ReadFile(fileName.c_str(),
		aiProcess_CalcTangentSpace |			// calculate tangents and bitangents if possible
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
		//aiProcess_MakeLeftHanded |
		//aiProcess_FlipUVs | 
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

	CollectBoneNode();
	CreateFullNode(m_aiScene->mRootNode, -1, m_fullNode);
	MarkParentsAnimation(m_fullNode);
	RemoveNoneAnimationBone(m_fullNode, m_reducedSkeleton);

	CreateMesh();
	CreateBone();
	CreateNode(m_aiScene->mRootNode, -1);
	CreateMeshBone(m_aiScene->mRootNode);
	CreateAnimation();

	return true;
}


// find all bones that influence the meshes first
void cAssimpLoader::CollectBoneNode()
{
	for (unsigned int m = 0; m < m_aiScene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_aiScene->mMeshes[m];
		assert(mesh);

		for (unsigned int b = 0; b < mesh->mNumBones; ++b)
		{
			aiBone* bone = mesh->mBones[b];
			assert(bone);
			m_aiBones[Str64(bone->mName.data).GetHashCode()] = bone;
		}
	}
}


// 노드를 기반으로 스켈레톤을 만든다.
void cAssimpLoader::CreateFullNode(const aiNode* node, int parent
	, vector<SkeletonNode>& result) const
{
	if (!node)
		return;

	Str64 nodeName = Str64(node->mName.data);
	map<hashcode, aiBone*>::const_iterator itBone = m_aiBones.find(nodeName.GetHashCode());
	bool isAnimated = itBone != m_aiBones.end();

	SkeletonNode _n =
	{
		node,
		isAnimated ? itBone->second : NULL,
		parent,
		nodeName,
		isAnimated
	};

	result.push_back(_n);
	int new_parent = result.size() - 1;

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		CreateFullNode(node->mChildren[i], new_parent, result);
}


// 자식노드가 애니메이션 노드라면, 부모도 애니메이션 노드가되게 플래그를 설정한다.
void  cAssimpLoader::MarkParentsAnimation(std::vector<SkeletonNode>& fullNodes) const
{
	for (unsigned int i = 0; i < fullNodes.size(); ++i)
	{
		SkeletonNode& currentNode = fullNodes[i];
		if (currentNode.animate)
		{
			char p = currentNode.parent;
			while (p >= 0)
			{
				SkeletonNode& n = fullNodes[p];
				if (n.animate) 
					break;

				n.animate = true;
				p = n.parent;
			}
		}
	}
}


// 에니메이션이 있는 노드만 추려서 리턴한다.
void cAssimpLoader::RemoveNoneAnimationBone(
	const std::vector<SkeletonNode>& fullNodes
	, std::vector<SkeletonNode>& reducedNodes) const
{
	map<int, int> nodeMapping; // old node id, new node id
	nodeMapping[-1] = -1;

	for (unsigned int i = 0; i < fullNodes.size(); ++i)
	{
		SkeletonNode n = fullNodes[i];
		if (n.animate)
		{
			n.parent = nodeMapping[n.parent];
			reducedNodes.push_back(n);

			nodeMapping[i] = reducedNodes.size() - 1;
		}
	}
}


void cAssimpLoader::CreateMesh()
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
		rawMesh->indices.reserve(mesh->mNumFaces * nidx);

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

		// has bumpmap, load tangent, binormal
		const bool isBumpMap = !rawMesh->mtrl.bumpMap.empty();
		if (isBumpMap)
		{
			rawMesh->tangent.reserve(mesh->mNumVertices);
			rawMesh->binormal.reserve(mesh->mNumVertices);
		}

		for (unsigned int x = 0; x < mesh->mNumVertices; ++x)
		{
			Vector3 position;
			Vector3 normal(0.0f, 0.0f, 0.0f);
			Vector3 tangent(0.0f, 0.0f, 0.0f);
			Vector3 binormal(0.0f, 0.0f, 0.0f);
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

			if (NULL != mesh->mTangents)
			{
				tangent.x = mesh->mTangents[x].x;
				tangent.y = mesh->mTangents[x].y;
				tangent.z = mesh->mTangents[x].z;
			}

			if (NULL != mesh->mBitangents)
			{
				binormal.x = mesh->mBitangents[x].x;
				binormal.y = mesh->mBitangents[x].y;
				binormal.z = mesh->mBitangents[x].z;
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
			if (isBumpMap)
			{
				rawMesh->tangent.push_back(tangent);
				rawMesh->binormal.push_back(binormal);
			}
		}
	}
}


void cAssimpLoader::CreateMaterial(const aiMesh *sourceMesh, OUT sMaterial &mtrl)
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
			StrPath fileName = szPath.data;
			mtrl.texture = fileName;
		}

		// Normal texture
		if (AI_SUCCESS == aiGetMaterialString(sourceMaterial, AI_MATKEY_TEXTURE_HEIGHT(0), &szPath))
		{
			StrPath fileName = szPath.data;
			mtrl.bumpMap = fileName;
		}

		// Specular texture
		if (AI_SUCCESS == aiGetMaterialString(sourceMaterial, AI_MATKEY_TEXTURE_SPECULAR(0), &szPath))
		{
			StrPath fileName = szPath.data;
			mtrl.specularMap = fileName;
		}		

		// Self Illumination texture
		if (AI_SUCCESS == aiGetMaterialString(sourceMaterial, AI_MATKEY_TEXTURE_EMISSIVE(0), &szPath))
		{
			StrPath fileName = szPath.data;
			mtrl.selfIllumMap = fileName;
		}

		// Glow texture
		if (AI_SUCCESS == aiGetMaterialString(sourceMaterial, AI_MATKEY_TEXTURE_SHININESS(0), &szPath))
		{
			StrPath fileName = szPath.data;
			mtrl.glowMap = fileName;
		}

	}
}


int cAssimpLoader::GetBoneId(const Str64 &boneName)
{
	for (u_int i = 0; i < m_reducedSkeleton.size(); ++i)
	{
		if (m_reducedSkeleton[i].name == boneName)
			return i;
	}
	return -1;
}


void cAssimpLoader::CreateBone()
{
	m_rawMeshes->bones.resize(m_reducedSkeleton.size());

	int i = 0;
	for (auto bone : m_reducedSkeleton)
	{
		sRawBone2 b;
		b.id = GetBoneId(bone.name);
		b.parentId = bone.parent;
		b.name = bone.name;

		assert(bone.node);
		
		b.localTm = *(Matrix44*)&bone.node->mTransformation;
		b.localTm.Transpose();

		if (bone.bone)
		{
			b.offsetTm = *(Matrix44*)&bone.bone->mOffsetMatrix;
			b.offsetTm.Transpose();
		}
		else
		{
			// not need offsetMatrix
		}

		m_rawMeshes->bones[i++] = b;
	}
}


void cAssimpLoader::CreateAnimation()
{
	RET(m_aiScene->mNumAnimations <= 0);

	// Read Sequence file
	// read '.seq' file
	// animation name, start time, end time
	struct sSequence
	{
		StrId name;
		float start;
		float end;
	};
	vector<sSequence> seqs;
	using namespace std;
	StrPath aniFileName = m_fileName.GetFileNameExceptExt2() + ".seq";
	ifstream ifs(aniFileName.c_str());
	if (ifs.is_open())
	{
		Str128 line;
		while (ifs.getline(line.m_str, sizeof(line.m_str)))
		{
			stringstream ss(line.c_str());
			StrId name;
			float start = -1, end = -1;
			ss >> name.m_str >> start >> end;

			if (name.empty())
				continue;

			const float fps = 1.f / 960.f;
			seqs.push_back({ name, start*fps, (end-start)*fps });
		}
	}
	else
	{
		const aiAnimation* sourceAnimation = m_aiScene->mAnimations[0];
		StrId name = StrId(m_fileName.GetFileName()) + "::" + sourceAnimation->mName.data;
		seqs.push_back({ name, 0, -1});
	}
	//

	m_rawAnies = new sRawAniGroup;

	for (size_t j = 0; j < seqs.size(); j++)
	{
		const sSequence &seq = seqs[j];
		const aiAnimation* sourceAnimation = m_aiScene->mAnimations[0];

		//animation.TicksPerSecond = static_cast<float>(sourceAnimation->mTicksPerSecond);
		//animation.Duration = static_cast<float>(sourceAnimation->mDuration);

		m_rawAnies->anies.push_back(sRawAni());
		sRawAni &ani = m_rawAnies->anies.back();

		ani.name = seq.name;
		ani.boneAnies.resize(m_rawMeshes->bones.size());
		ani.start = FLT_MAX;
		ani.end = -FLT_MAX;

		for (UINT a = 0; a < sourceAnimation->mNumChannels; a++)
		{
			Str64 boneName = sourceAnimation->mChannels[a]->mNodeName.data;
			const int boneId = GetBoneId(boneName);
			if (boneId < 0)
				continue;

			int posCnt = 0, rotCnt=0, scaleCnt=0;
			for (unsigned int i = 0; i < sourceAnimation->mChannels[a]->mNumPositionKeys; i++)
			{
				aiVectorKey &key = sourceAnimation->mChannels[a]->mPositionKeys[i];
				const float t = (float)key.mTime;
				if ((seq.start >= 0) && (seq.end >= 0))
				{
					if ((t < seq.start) || (t > seq.end))
						continue;
				}
				++posCnt;
			}

			for (unsigned int i = 0; i < sourceAnimation->mChannels[a]->mNumRotationKeys; i++)
			{
				aiQuatKey &key = sourceAnimation->mChannels[a]->mRotationKeys[i];
				const float t = (float)key.mTime;
				if ((seq.start >= 0) && (seq.end >= 0))
				{
					if ((t < seq.start) || (t > seq.end))
						continue;
				}
				++rotCnt;
			}

			for (unsigned int i = 0; i < sourceAnimation->mChannels[a]->mNumScalingKeys; i++)
			{
				aiVectorKey &key = sourceAnimation->mChannels[a]->mScalingKeys[i];
				const float t = (float)key.mTime;
				if ((seq.start >= 0) && (seq.end >= 0))
				{
					if ((t < seq.start) || (t > seq.end))
						continue;
				}
				++scaleCnt;
			}


			sBoneAni &boneAni = ani.boneAnies[boneId];
			boneAni.name = boneName.c_str();
			boneAni.pos.reserve(posCnt);
			boneAni.rot.reserve(rotCnt);
			boneAni.scale.reserve(scaleCnt);

			double minT = FLT_MAX;
			double maxT = -FLT_MAX;

			// Position
			Vector3 prevPos(0,0,0);
			for (unsigned int i = 0; i < sourceAnimation->mChannels[a]->mNumPositionKeys; i++)
			{
				aiVectorKey &key = sourceAnimation->mChannels[a]->mPositionKeys[i];
				float t = (float)key.mTime - seq.start;
				if (seq.end >= 0)
				{
					const bool isLast = i == (sourceAnimation->mChannels[a]->mNumPositionKeys - 1);
					if ((t > seq.end) || isLast)
					{
						if (boneAni.pos.empty())
						{
							if (0 < minT)
								minT = 0;
							if (0 > maxT)
								maxT = 0;

							sKeyPos pos;
							pos.t = 0;
							pos.p = Vector3(key.mValue.x, key.mValue.y, key.mValue.z);
							boneAni.pos.push_back(pos);

							pos.t = 0.0333f;
							boneAni.pos.push_back(pos);
						}

						break;
					}

					if ((t < 0) && (abs(t) > 0.0001f))
						continue;
				}

				t = max(0, t);

				if (t < minT)
					minT = t;
				if (t > maxT)
					maxT = t;

				Vector3 aniPos(key.mValue.x, key.mValue.y, key.mValue.z);
				if (!boneAni.pos.empty() && (prevPos == aniPos))
					continue;
				prevPos = aniPos;

				sKeyPos pos;
				pos.t = t;
				pos.p = aniPos;
				boneAni.pos.push_back(pos);
			}

			// Rotation
			Quaternion prevRot;
			for (unsigned int i = 0; i < sourceAnimation->mChannels[a]->mNumRotationKeys; i++)
			{
				aiQuatKey &key = sourceAnimation->mChannels[a]->mRotationKeys[i];
				float t = (float)key.mTime - seq.start;
				if (seq.end >= 0)
				{
					const bool isLast = i == (sourceAnimation->mChannels[a]->mNumRotationKeys - 1);
					if ((t > seq.end) || isLast)
					{
						if (boneAni.rot.empty())
						{
							if (0 < minT)
								minT = 0;
							if (0 > maxT)
								maxT = 0;

							sKeyRot rot;
							rot.t = 0;
							rot.q = Quaternion(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w);
							boneAni.rot.push_back(rot);

							rot.t = 0.0333f;
							boneAni.rot.push_back(rot);
						}

						break;
					}

					if ((t < 0) && (abs(t) > 0.0001f))
						continue;
				}

				t = max(0, t);

				if (t < minT)
					minT = t;
				if (t > maxT)
					maxT = t;

				Quaternion aniRot(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w);
				if (!boneAni.rot.empty() && (prevRot == aniRot))
					continue;
				prevRot = aniRot;

				sKeyRot rot;
				rot.t = (float)key.mTime - seq.start;
				rot.q = aniRot;
				boneAni.rot.push_back(rot);
			}

			// Scale
			Vector3 prevScale;
			for (unsigned int i = 0; i < sourceAnimation->mChannels[a]->mNumScalingKeys; i++)
			{
				aiVectorKey &key = sourceAnimation->mChannels[a]->mScalingKeys[i];
				float t = (float)key.mTime - seq.start;
				if (seq.end >= 0)
				{
					const bool isLast = i == (sourceAnimation->mChannels[a]->mNumScalingKeys - 1);
					if ((t > seq.end) || isLast)
					{
						if (boneAni.scale.empty())
						{
							if (0 < minT)
								minT = 0;
							if (0 > maxT)
								maxT = 0;

							sKeyScale scale;
							scale.t = 0;
							scale.s = Vector3(key.mValue.x, key.mValue.y, key.mValue.z);
							boneAni.scale.push_back(scale);

							scale.t = 0.0333f;
							boneAni.scale.push_back(scale);
						}

						break;
					}

					if ((t < 0) && (abs(t) > 0.0001f))
						continue;
				}

				t = max(0, t);

				if (t < minT)
					minT = t;
				if (t > maxT)
					maxT = t;

				const Vector3 aniScale(key.mValue.x, key.mValue.y, key.mValue.z);
				if (!boneAni.scale.empty() && (prevScale == aniScale))
					continue;
				prevScale = aniScale;

				sKeyScale scale;
				scale.t = (float)key.mTime - seq.start;
				scale.s = aniScale;
				boneAni.scale.push_back(scale);
			}

			boneAni.start = (float)minT;
			boneAni.end = (float)maxT;

			ani.start = min((float)minT, ani.start);
			ani.end = max((float)maxT, ani.end);
		}

		int a = 0;
	}
}


// Create sRawNode and Children node
// Update Local Transform
void cAssimpLoader::CreateNode(aiNode* node
	, const int parentNodeIdx //=-1
)
{
	m_rawMeshes->nodes.push_back(sRawNode());
	sRawNode *newNode = &m_rawMeshes->nodes.back();
	const int nodeIdx = m_rawMeshes->nodes.size() - 1;
	newNode->name = node->mName.data;
	newNode->localTm = *(Matrix44*)&node->mTransformation;
	newNode->localTm.Transpose();

	if (parentNodeIdx >= 0)
	{
		sRawNode &parentNode = m_rawMeshes->nodes[parentNodeIdx];
		parentNode.children.push_back(nodeIdx);
	}

	for (u_int m = 0; m < node->mNumMeshes; ++m)
	{
		sRawMesh2 &mesh = m_rawMeshes->meshes[node->mMeshes[m]];

		newNode->meshes.push_back(node->mMeshes[m]);
	}

	for (u_int c = 0; c < node->mNumChildren; c++)
		CreateNode(node->mChildren[c], nodeIdx);
}


void cAssimpLoader::CreateMeshBone(aiNode* node)
{
	for (u_int m = 0; m < node->mNumMeshes; ++m)
	{
		aiMesh *mesh = m_aiScene->mMeshes[node->mMeshes[m]];
		sRawMesh2 &rawMesh = m_rawMeshes->meshes[node->mMeshes[m]];

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

