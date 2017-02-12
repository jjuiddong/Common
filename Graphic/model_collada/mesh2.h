//
// 2017-02-10, jjuiddong
// collada export mesh
// tree architecture mesh
//
#pragma once


namespace graphic {

	class cBoneNode2;

	class cMesh2
	{
	public:
		cMesh2();
		virtual ~cMesh2();

		bool Create(cRenderer &renderer, const sRawMesh2 mesh, vector<Matrix44> *tmPalette);
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		bool Update(const float deltaSeconds);
		void Clear();


	protected:
		void CreateMaterials(cRenderer &renderer, const sRawMesh2 &rawMesh);


	public:
		vector<cMaterial> m_mtrls;
		vector<cTexture*>m_colorMap;  // reference
		vector<cTexture*>m_normalMap;  // reference
		vector<cTexture*>m_specularMap;  // reference
		vector<cTexture*>m_selfIllumMap;  // reference
		vector<cBoneNode2*> m_bones;
		cMeshBuffer *m_buffers;
		cMeshBuffer *m_skinnedBuffers;
		vector<Matrix44> *m_tmPalette; // reference
	};
	
}
