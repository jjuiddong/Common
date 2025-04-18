//
// 2017-02-16, jjuiddong
// Assimp export mesh
// tree architecture mesh
//
// 2019-11-13
//	- texture file loading
//		- 1. search texture in mesh directory
//		- 2. search media directory
//
#pragma once


namespace graphic {

	using namespace common;

	class cMesh
	{
	public:
		cMesh();
		virtual ~cMesh();

		bool Create(cRenderer &renderer, INOUT sRawMesh2 &mesh
			, const bool calculateTangentBinormal=false
			, const StrPath &modelFileName = "");

		void Render(cRenderer &renderer
			, const char *techniqueName
			, cSkeleton *skeleton
			, const XMMATRIX &nodeParentTm = XMIdentity
			, const XMMATRIX &parentTm = XMIdentity
		);

		void RenderInstancing(cRenderer &renderer
			, const char *techniqueName
			, cSkeleton *skeleton
			, const int count
			, const Matrix44 *transforms
			, const XMMATRIX &nodeParentTm = XMIdentity
			, const XMMATRIX &parentTm = XMIdentity
		);

		void RenderTessellation(cRenderer &renderer
			, const char *techniqueName
			, const int controlPointCount
			, cSkeleton *skeleton
			, const XMMATRIX &parentTm = XMIdentity
			, const XMMATRIX &transform = XMIdentity
		);

		inline bool IsVisible() const;
		inline int SetRenderFlag(const eRenderFlag::Enum val, const bool enable);
		inline bool IsRenderFlag(const eRenderFlag::Enum val);

		void Clear();


	protected:
		void CreateMaterials(cRenderer &renderer, const sRawMesh2 &rawMesh
			, const StrPath &modelFileName = "");
		void UpdateConstantBuffer(cRenderer &renderer, const char *techniqueName
			, cSkeleton *skeleton
			, const XMMATRIX &nodeParentTm
			, const XMMATRIX &parentTm
			, const bool isInstancing = false
			, const bool isTessellation = false
		);

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
		int m_renderFlags; // default : eRenderFlag::VISIBLE | eRenderFlag::NOALPHABLEND
		Transform m_transform;
		vector<cMaterial> m_mtrls;
		vector<cTexture*> m_colorMap;  // reference
		vector<cTexture*> m_normalMap;  // reference
		vector<cTexture*> m_specularMap;  // reference
		vector<cTexture*> m_selfIllumMap;  // reference
		vector<sMeshBone> m_bones; // mesh bone
		cShader11 *m_shader;
		bool m_isBeginShader; // default: true, for external constant buffer initialize
		cMeshBuffer *m_buffers;
	};


	inline bool cMesh::IsVisible() const { return (m_renderFlags & eRenderFlag::VISIBLE); }
	inline int cMesh::SetRenderFlag(const eRenderFlag::Enum val, const bool enable) {
		return (enable) ? (m_renderFlags |= val) : (m_renderFlags &= ~val);
	}
	inline bool cMesh::IsRenderFlag(const eRenderFlag::Enum val) {
		return (m_renderFlags & val) ? true : false;
	}
}
