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
	using namespace Assimp;

	class cAssimpLoader
	{
	public:
		struct SkeletonNode
		{
			const aiNode* node;
			const aiBone* bone;
			int parent;
			Str64 name;
			bool used;
		};

		cAssimpLoader();
		virtual ~cAssimpLoader();
		bool Create(const StrPath &fileName);


	protected:
		void FindBoneNode();
		void CreateSimpleBones(const aiNode* node, int parent, const map<hashcode, aiBone*>& animatedNodes
			, vector<SkeletonNode>& result) const;
		void MarkParents(std::vector<SkeletonNode>& hierarchy) const;
		void FilterHierarchy(const std::vector<SkeletonNode>& fullHierarchy, std::vector<SkeletonNode>& result) const;
		void CreateMesh();
		void CreateMaterial(const aiMesh *sourceMesh, OUT sMaterial &mtrl);
		void CreateBone();
		void CreateMeshBone(aiNode* node);
		void CreateNode(aiNode* node, const int parentNodeIdx=-1);
		void CreateAnimation();
		int GetBoneId(const Str64 &boneName);


	public:
		const aiScene* m_aiScene;
		int m_numMeshes;
		int m_numMaterials;
		int m_numTextures;
		int m_numAnimations;
		bool m_hasAnimations;

		StrPath m_fileName;
		map<hashcode, aiBone*> m_aiBones;
		vector<SkeletonNode> m_fullHierarchy;
		vector<SkeletonNode> m_reducedHierarchy;
		sRawMeshGroup2 *m_rawMeshes;
		sRawAniGroup *m_rawAnies;
	};

}
