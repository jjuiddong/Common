//
// 2017-02-16, jjuiddong
// Assimp export loader
//
// 2017-08-10
//	- Upgrade DX11
//
#pragma once

// Assimpl include
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace graphic
{
	using namespace common;
	using namespace Assimp;

	class cAssimpLoader
	{
	public:
		// aiNode 구조와 동일한 스켈레톤 구조체
		struct SkeletonNode
		{
			const aiNode* node;
			const aiBone* bone; // mesh에 소속된 bone, node의 대표 bone
			int parent;
			Str64 name;
			bool animate;
		};

		cAssimpLoader();
		virtual ~cAssimpLoader();
		bool Create(const StrPath &fileName);


	protected:
		void CollectBoneNode();
		void CreateFullNode(const aiNode* node, int parent, vector<SkeletonNode>& result) const;
		void MarkParentsAnimation(std::vector<SkeletonNode>& fullNodes) const;
		void RemoveNoneAnimationBone(const std::vector<SkeletonNode>& fullNodes, std::vector<SkeletonNode>& reducedNodes) const;
		void CreateMesh();
		void CreateMaterial(const aiMesh *sourceMesh, OUT sMaterial &mtrl);
		void CreateBone();
		void CreateMeshBone(aiNode* node);
		void CreateNode(aiNode* node, const int parentNodeIdx=-1);
		void CreateAnimation();
		void LoadOptionFile();
		int GetBoneId(const Str64 &boneName);


	public:
		const aiScene* m_aiScene;
		int m_numMeshes;
		int m_numMaterials;
		int m_numTextures;
		int m_numAnimations;
		bool m_hasAnimations;

		StrPath m_fileName;
		map<hashcode, aiBone*> m_aiBones; // total bones
		vector<SkeletonNode> m_fullNode;
		vector<SkeletonNode> m_reducedSkeleton;
		sRawMeshGroup2 *m_rawMeshes;
		sRawAniGroup *m_rawAnies;
	};

}
