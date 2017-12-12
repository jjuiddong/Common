#pragma once


namespace graphic
{

	struct sVertex
	{
		Vector3 p;
	};


	struct sVertexDiffuse
	{
		Vector3 p;
		Vector4 c;
	};


	struct sVertexTex
	{
		Vector3 p;
		float u,v;

		sVertexTex() {}
		sVertexTex(float x0, float y0, float z0, float u0, float v0) : p(x0,y0,z0), u(u0), v(v0) {}
	};


	struct sVertexTex2
	{
		Vector4 p;
		float u, v;
	};


	struct sVertexTexRhw
	{
		Vector4 p;
		float u, v;

		sVertexTexRhw() {}
		sVertexTexRhw(float x0, float y0, float z0, float w0, float u0, float v0) : p(x0, y0, z0, w0), u(u0), v(v0) {}
	};


	struct sVertexNorm
	{
		Vector3 p;
		Vector3 n;
	};

	struct sVertexNormTex
	{
		Vector3 p;
		Vector3 n;
		float u,v;
	};


	// 스키닝 + 범프 매핑
	struct sVertexNTBTex
	{
		Vector3 p;
		Vector3 n;
		float u,v;
		Vector3 t; // tangent;
	};

	// vertex skinning
	// using texcoord
	struct sVertexNormTexSkin
	{
		Vector3 p;
		Vector3 n;
		float u,v;
		float weights[4];
		float matrixIndices[4];
	};


	// vertex skinning
	// using D3DFVF_XYZB5 | D3DFVF_LASTBETA_D3DCOLOR
	struct sVertexNormTexSkin2
	{
		Vector3 p;
		float weights[4];
		DWORD matrixIndices;
		Vector3 n;
		float u,v;	
	};


	// vertex skinning
	// using texcoord, ksm file format
	struct sVertexNormTexSkin3
	{
		Vector3 p;
		Vector3 n;
		float u, v;
		float weights[3];
		BYTE matrixIndices[4];
	};
}
