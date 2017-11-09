//
// 2017-02-16, jjuiddong
// Assimp export mesh
// tree architecture mesh
//
#pragma once


namespace graphic {

	class cMesh
	{
	public:
		cMesh();
		virtual ~cMesh();

		bool Create(cRenderer &renderer, INOUT sRawMesh2 &mesh
			, const bool calculateTangentBinormal=false);

		void Render(cRenderer &renderer
			, const char *techniqueName
			, cSkeleton *skeleton
			, const XMMATRIX &parentTm = XMIdentity
			, const XMMATRIX &transform = XMIdentity
		);

		void RenderInstancing(cRenderer &renderer
			, const char *techniqueName
			, cSkeleton *skeleton
			, const int count, const XMMATRIX &parentTm = XMIdentity);
		void Clear();


	protected:
		void CreateMaterials(cRenderer &renderer, const sRawMesh2 &rawMesh);
		void UpdateConstantBuffer(cRenderer &renderer, const char *techniqueName
			, cSkeleton *skeleton, const XMMATRIX &parentTm, const XMMATRIX &transform);
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
		bool m_isShow;
		int m_renderFlags; // default : eRenderFlag::VISIBLE | eRenderFlag::NOALPHABLEND
		Transform m_transform;
		vector<cMaterial> m_mtrls;
		vector<cTexture*> m_colorMap;  // reference
		vector<cTexture*> m_normalMap;  // reference
		vector<cTexture*> m_specularMap;  // reference
		vector<cTexture*> m_selfIllumMap;  // reference
		vector<sMeshBone> m_bones; // mesh bone
		cMeshBuffer *m_buffers;
	};
	
}
