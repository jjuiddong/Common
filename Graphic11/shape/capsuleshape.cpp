
#include "stdafx.h"
#include "capsuleshape.h"

using namespace graphic;


cCapsuleShape::cCapsuleShape()
{
}

cCapsuleShape::~cCapsuleShape()
{
}


//
// 0µµ -> xyz = (1,0,0)
// 90µµ -> xyz = (0,0,1)
// 180µµ -> xyz = (-1,0,0)
//
bool cCapsuleShape::Create(cRenderer &renderer, const float radius
	, const float halfHeight
	, const int stacks
	, const int slices
	, const int vtxType // = (eVertexType::POSITION | eVertexType::NORMAL| eVertexType::COLOR)
	, const cColor &color // = cColor::BLACK
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return false; // already exist

	const int numSphereVertices = (slices * 2 + 1)*(stacks + 1);
	const int numSphereIndices = slices * 2 * stacks * 6;
	const int numConeVertices = (slices * 2 + 1) * 2;
	const int numConeIndices = slices * 2 * 6;
	const int numCapsuleVertices = 2 * numSphereVertices + numConeVertices;
	const int numCapsuleIndices = 2 * numSphereIndices + numConeIndices;

	vector<Vector3> spherePositions(numSphereVertices);
	vector<WORD> sphereIndices(numSphereIndices);
	GenerateSphereMesh(slices, stacks, &spherePositions[0], &sphereIndices[0]);

	vector<Vector3> conePositions(numConeVertices);
	vector<unsigned short> coneIndices(numConeIndices);
	GenerateConeMesh(slices, &conePositions[0], &coneIndices[0], numSphereVertices * 2);

	const int vtxCount = numCapsuleVertices;
	const int faceCount = numCapsuleIndices / 3;
	const Vector4 vColor = color.GetColor();

	cVertexLayout vtxLayout;
	vtxLayout.Create(vtxType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	vector<BYTE> vertices(vtxCount * vertexStride);
	vector<WORD> indices(faceCount * 3);
	BYTE *pVertex = &vertices[0];
	WORD *pIndices = &indices[0];

	const float radius0 = radius;
	const float radius1 = radius;

	const float radii[2] = { radius1, radius0 };
	const float offsets[2] = { halfHeight, -halfHeight };

	// write two copies of the sphere mesh scaled and offset appropriately
	for (int s = 0; s < 2; ++s)
	{
		const float r = radii[s];
		const float offset = offsets[s];

		for (int i = 0; i < numSphereVertices; ++i)
		{
			Vector3 p = spherePositions[i] * r;
			p.x += offset;

			if (vtxType & eVertexType::POSITION)
				*(Vector3*)(pVertex + posOffset) = p;
			if (vtxType & eVertexType::COLOR)
				*(Vector4*)(pVertex + colorOffset) = vColor;
			if (vtxType & eVertexType::NORMAL)
				*(Vector3*)(pVertex + normOffset) = spherePositions[i].Normal();
			//if (vtxType & eVertexType::TEXTURE0)
			//	*(Vector2*)(pVertex + texOffset) = Vector2(u1, v1);

			pVertex += vertexStride;
		}
	}

	// calculate cone angle
	float cosTheta = 0.0f;

	if (halfHeight > 0.0f)
		cosTheta = (radius0 - radius1) / (halfHeight*2.0f);

	// scale factor for normals to avoid re-normalizing each time
	float nscale = (float)sqrt(1.0f - cosTheta * cosTheta);

	// rotation z axis 90 degree
	Quaternion rot;
	rot.SetRotationZ(MATH_PI / 2.f);
	Matrix44 rTm = rot.GetMatrix();

	for (int s = 0; s < 2; ++s)
	{
		const float y = radii[s] * cosTheta;
		const float r = (float)sqrt(radii[s] * radii[s] - y * y);
		const float offset = offsets[s] + y;

		for (int i = 0; i < numConeVertices / 2; ++i)
		{
			Vector3 p = conePositions[i] * r;
			p.y += offset;
			p = p * rTm;

			Vector3 n = conePositions[i] * nscale;
			n.y = cosTheta;
			n = n * rTm;

			if (vtxType & eVertexType::POSITION)
				*(Vector3*)(pVertex + posOffset) = p;
			if (vtxType & eVertexType::COLOR)
				*(Vector4*)(pVertex + colorOffset) = vColor;
			if (vtxType & eVertexType::NORMAL)
				*(Vector3*)(pVertex + normOffset) = n.Normal();
			//if (vtxType & eVertexType::TEXTURE0)
			//	*(Vector2*)(pVertex + texOffset) = Vector2(u1, v1);

			pVertex += vertexStride;
		}
	}

	if (pIndices)
	{
		// first sphere
		for (int i = 0; i < numSphereIndices; ++i)
			pIndices[i] = sphereIndices[i];

		pIndices += numSphereIndices;

		// second sphere
		for (int i = 0; i < numSphereIndices; ++i)
			pIndices[i] = sphereIndices[i] + numSphereVertices;

		pIndices += numSphereIndices;

		// cone indices
		for (int i = 0; i < numConeIndices; ++i)
			pIndices[i] = coneIndices[i];
	}

	m_vtxBuff.Create(renderer, vtxCount, vertexStride, &vertices[0]);
	m_idxBuff.Create(renderer, faceCount, (BYTE*)&indices[0]);
	m_vtxType = vtxType;
	return true;
}


void cCapsuleShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);
}


void cCapsuleShape::GenerateSphereMesh(int slices, int stacks, Vector3* positions, WORD* indices)
{
	const float thetaStep = MATH_PI / stacks;
	const float phiStep = (MATH_PI * 2.f) / (slices * 2);
	float theta = 0.0f;

	// generate vertices
	for (int y = 0; y <= stacks; ++y)
	{
		float phi = 0.0f;

		const float cosTheta = (float)cos(theta);
		const float sinTheta = (float)sin(theta);

		for (int x = 0; x <= slices * 2; ++x)
		{
			const float cosPhi = (float)cos(phi);
			const float sinPhi = (float)sin(phi);

			const Vector3 p(cosPhi*sinTheta, cosTheta, sinPhi*sinTheta);
			*(positions++) = p;
			phi += phiStep;
		}

		theta += thetaStep;
	}

	const int numRingQuads = 2 * slices;
	const int numRingVerts = 2 * slices + 1;
	// add faces
	for (int y = 0; y < stacks; ++y)
	{
		for (int i = 0; i < numRingQuads; ++i)
		{
			// add a quad
			*(indices++) = (y + 0)*numRingVerts + i;
			*(indices++) = (y + 1)*numRingVerts + i + 1;
			*(indices++) = (y + 1)*numRingVerts + i;

			*(indices++) = (y + 1)*numRingVerts + i + 1;
			*(indices++) = (y + 0)*numRingVerts + i;
			*(indices++) = (y + 0)*numRingVerts + i + 1;
		}
	}
}

void cCapsuleShape::GenerateConeMesh(int slices, Vector3* positions, WORD* indices, int offset)
{
	// generate vertices
	const float phiStep = (MATH_PI * 2.f) / (slices * 2);
	float phi = 0.0f;

	// generate two rings of vertices for the cone ends
	for (int i = 0; i < 2; ++i)
	{
		for (int x = 0; x <= slices * 2; ++x)
		{
			const float cosPhi = (float)cos(phi);
			const float sinPhi = (float)sin(phi);

			const Vector3 p(cosPhi, 0.0f, sinPhi);
			*(positions++) = p;

			phi += phiStep;
		}
	}

	const int numRingQuads = 2 * slices;
	const int numRingVerts = 2 * slices + 1;

	// add faces
	for (int i = 0; i < numRingQuads; ++i)
	{
		// add a quad
		*(indices++) = offset + i;
		*(indices++) = offset + numRingVerts + i + 1;
		*(indices++) = offset + numRingVerts + i;

		*(indices++) = offset + numRingVerts + i + 1;
		*(indices++) = offset + i;
		*(indices++) = offset + i + 1;
	}
}
