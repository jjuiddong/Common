//
// 2017-02-16, jjuiddong
// collada export loader
//
#pragma once

// Assimpl include
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace graphic
{
	using namespace Assimp;

	class cColladaLoader
	{
	public:
		struct SkeletonNode
		{
			const aiNode* node;
			const aiBone* bone;
			int parent;
			string name;
			bool used;
		};

		cColladaLoader();
		virtual ~cColladaLoader();
		bool Create(const string &fileName);


	protected:
		void FindBoneNode();
		void CreateSimpleBones(const aiNode* node, int parent, const map<string, aiBone*>& animatedNodes, 
			vector<SkeletonNode>& result) const;
		void  MarkParents(std::vector<SkeletonNode>& hierarchy) const;
		void FilterHierarchy(const std::vector<SkeletonNode>& fullHierarchy, std::vector<SkeletonNode>& result) const;
		void CreateMesh();
		void CreateMaterial(const aiMesh *sourceMesh, OUT sMaterial &mtrl);
		void CreateBone();
		void CreateMeshBone(aiNode* node);
		void CreateNode(aiNode* node);
		void CreateAnimation();
		int GetBoneId(const string &boneName);


	public:
		const aiScene* m_aiScene;
		int m_numMeshes;
		int m_numMaterials;
		int m_numTextures;
		int m_numAnimations;
		bool m_hasAnimations;

		string m_fileName;
		map<string, aiBone*> m_aiBones;
		vector<SkeletonNode> m_fullHierarchy;
		vector<SkeletonNode> m_reducedHierarchy;
		sRawMeshGroup2 *m_rawMeshes;
		sRawAniGroup *m_rawAnies;
	};

}
