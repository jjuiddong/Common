#pragma once


namespace graphic
{

	struct sMaterial
	{
		Vector4 ambient;
		Vector4 diffuse;
		Vector4 specular;
		Vector4 emissive;
		float power;
		StrPath directoryPath; // 텍스쳐가 저장된 디렉토리 경로를 저장한다.
		StrPath texture;
		StrPath bumpMap;
		StrPath specularMap;
		StrPath selfIllumMap;
		StrPath glowMap;

		sMaterial() {}
		sMaterial(const sMaterial &rhs);
		sMaterial& operator=(const sMaterial &rhs);
	};


	// 속성버퍼 정보를 표현함.
	struct sAttribute
	{
		int attribId;
		int faceStart;
		int faceCount;
		int vertexStart;
		int vertexCount;
	};


	struct sWeight
	{
		int bone;
		float weight;
	};


	struct sVertexWeight
	{
		int vtxIdx;
		int size;
		sWeight w[6];
	};


	//------------------------------------------------------------------------------------------------------
	// Assimp Format Version
	struct sRawBone2
	{
		int id;
		int parentId;
		StrId name;
		Matrix44 localTm;  // aiNode mTransformation
		Matrix44 offsetTm;	// aiNode -> bone -> mOffsetMatrix
	};

	struct sMeshBone
	{
		int id; // global bone palette id
		StrId name;
		Matrix44 offsetTm;
	};

	struct sRawMesh2
	{
		StrId name;
		int renderFlags; // default: eRenderFlag::VISIBLE | eRenderFlag::NOALPHABLEND
		vector<Vector3> vertices;
		vector<Vector3> normals; // vertex 갯수만큼 저장된다.
		vector<Vector3> tangent; // vertex 갯수만큼 저장된다.
		vector<Vector3> binormal; // vertex 갯수만큼 저장된다.
		vector<Vector3> tex; // vertex 갯수만큼 저장된다.
		vector<int> indices;
		vector<sAttribute> attributes;
		vector<sVertexWeight> weights; // vertex 갯수만큼 저장된다.
		vector<sMeshBone> bones;
		sMaterial mtrl;
	};

	struct sRawNode
	{
		StrId name;
		vector<int> meshes; // sRawMeshGroup2::meshes index
		vector<int> children; // sRawMeshGroup2::nodes index
		Matrix44 localTm;
	};

	struct sRawMeshGroup2
	{
		StrId name;
		StrId animationName; // set by Resource Manager
		vector<sRawMesh2> meshes;
		vector<sRawBone2> bones;
		vector<sRawNode> nodes;
	};
	//------------------------------------------------------------------------------------------------------


}
