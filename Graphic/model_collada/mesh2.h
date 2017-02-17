//
// 2017-02-16, jjuiddong
// collada export mesh
// tree architecture mesh
//
#pragma once


namespace graphic {

	class cMesh2
	{
	public:
		cMesh2();
		virtual ~cMesh2();

		bool Create(cRenderer &renderer, const sRawMesh2 mesh, cSkeleton *skeleton);
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		bool Update(const float deltaSeconds);
		void Clear();


	protected:
		void CreateMaterials(cRenderer &renderer, const sRawMesh2 &rawMesh);


	public:
		string m_name;
		Matrix44 m_localTm;
		cSkeleton *m_skeleton; // reference
		vector<cMaterial> m_mtrls;
		vector<cTexture*>m_colorMap;  // reference
		vector<cTexture*>m_normalMap;  // reference
		vector<cTexture*>m_specularMap;  // reference
		vector<cTexture*>m_selfIllumMap;  // reference
		vector<sMeshBone> m_bones;
		cMeshBuffer *m_buffers;
		cMeshBuffer *m_skinnedBuffers;
	};
	
}
