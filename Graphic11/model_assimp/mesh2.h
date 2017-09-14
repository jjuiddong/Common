//
// 2017-02-16, jjuiddong
// Assimp export mesh
// tree architecture mesh
//
#pragma once


namespace graphic {

	class cMesh2
	{
	public:
		cMesh2();
		virtual ~cMesh2();

		bool Create(cRenderer &renderer, INOUT sRawMesh2 &mesh, cSkeleton *skeleton, const bool calculateTangentBinormal=false);
		void Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity);
		void RenderInstancing(cRenderer &renderer, const int count, const XMMATRIX &parentTm = XMIdentity);
		void Clear();


	protected:
		void CreateMaterials(cRenderer &renderer, const sRawMesh2 &rawMesh);
		void UpdateConstantBuffer(cRenderer &renderer, const XMMATRIX &parentTm);
		void CalculateModelVectors(INOUT graphic::sRawMesh2 &mesh);
		void CalculateTangentBinormal(
			const graphic::sVertexNormTex &vertex1
			, const graphic::sVertexNormTex &vertex2
			, const graphic::sVertexNormTex &vertex3
			, OUT Vector3& tangent
			, OUT Vector3& binormal
		);


	public:
		StrId m_name;
		Matrix44 m_localTm;
		Transform m_transform;
		cSkeleton *m_skeleton; // reference
		vector<cMaterial> m_mtrls;
		vector<cTexture*> m_colorMap;  // reference
		vector<cTexture*> m_normalMap;  // reference
		vector<cTexture*> m_specularMap;  // reference
		vector<cTexture*> m_selfIllumMap;  // reference
		vector<sMeshBone> m_bones; // mesh bone
		vector<Matrix44> m_tmPose;
		cMeshBuffer *m_buffers;
	};
	
}
