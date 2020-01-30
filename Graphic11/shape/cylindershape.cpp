
#include "stdafx.h"
#include "cylindershape.h"

using namespace graphic;


cCylinderShape::cCylinderShape()
{
}

cCylinderShape::~cCylinderShape()
{
}


//
// 0µµ -> xyz = (1,0,0)
// 90µµ -> xyz = (0,0,1)
// 180µµ -> xyz = (-1,0,0)
//
bool cCylinderShape::Create(cRenderer &renderer, const float radius
	, const float height
	, const int slices
	, const int vtxType // = (eVertexType::POSITION | eVertexType::NORMAL| eVertexType::COLOR)
	, const cColor &color // = cColor::BLACK
	, const bool isHead //= true
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return false; // already exist

	m_radius = radius;
	m_height = height;

	const int numConeVertices = (slices * 2 + 1) * 2;
	const int numCircleVertices = isHead ? (slices * 2 + 1) * 2 : 0;
	const int numConeIndices = slices * 2 * 6;
	const int numCircleIndices = isHead ? (slices * 2 + 1 - 2) * 2 * 3 : 0;

	vector<Vector3> conePositions(numConeVertices + numCircleVertices);
	vector<unsigned short> coneIndices(numConeIndices + numCircleIndices);
	GenerateConeMesh(slices, &conePositions[0], &coneIndices[0], 0, isHead);

	const int vtxCount = numConeVertices + numCircleVertices;
	const int faceCount = numConeIndices / 3 + numCircleIndices / 3;
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
	const float offsets[2] = { height/2.f, -height/2.f };

	// calculate cone angle
	float cosTheta = 0.0f;

	if (height > 0.0f)
		cosTheta = (radius0 - radius1) / height;

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

	// head circle vertex?
	if (isHead)
	{
		for (int s = 0; s < 2; ++s)
		{
			const float y = radii[s] * cosTheta;
			const float r = (float)sqrt(radii[s] * radii[s] - y * y);
			const float offset = offsets[s] + y;

			for (int i = 0; i < numCircleVertices / 2; ++i)
			{
				Vector3 p = conePositions[i] * r;
				p.y += offset;
				p = p * rTm;

				// x, -x axis normal
				Vector3 n = (s == 0) ? Vector3(-1, 0, 0) : Vector3(1, 0, 0);

				if (vtxType & eVertexType::POSITION)
					*(Vector3*)(pVertex + posOffset) = p;
				if (vtxType & eVertexType::COLOR)
					*(Vector4*)(pVertex + colorOffset) = vColor;
				if (vtxType & eVertexType::NORMAL)
					*(Vector3*)(pVertex + normOffset) = n;
				//if (vtxType & eVertexType::TEXTURE0)
				//	*(Vector2*)(pVertex + texOffset) = Vector2(u1, v1);

				pVertex += vertexStride;
			}
		}
	}

	if (pIndices)
	{
		// cone indices
		for (int i = 0; i < numConeIndices + numCircleIndices; ++i)
			pIndices[i] = coneIndices[i];
	}

	m_vtxBuff.Create(renderer, vtxCount, vertexStride, &vertices[0]);
	m_idxBuff.Create(renderer, faceCount, (BYTE*)&indices[0]);
	m_vtxType = vtxType;
	return true;
}


void cCylinderShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);
}


void cCylinderShape::GenerateConeMesh(int slices, Vector3* positions, WORD* indices, int offset
	, const bool isHead //= true
)
{
	// generate vertices
	const float phiStep = (MATH_PI * 2.f) / (slices * 2);
	float phi = 0.0f;

	// generate two rings of vertices for the cone ends
	for (int i = 0; i < 2; ++i)
	{
		for (int x = 0; x <= slices * 2; ++x)
		{
			const Vector3 p((float)cos(phi), 0.0f, (float)sin(phi));
			*(positions++) = p;
			phi += phiStep;
		}
	}

	if (isHead)
	{
		// generate two rings of vertices for the circle head
		for (int i = 0; i < 2; ++i)
		{
			for (int x = 0; x <= slices * 2; ++x)
			{
				const Vector3 p((float)cos(phi), 0.0f, (float)sin(phi));
				*(positions++) = p;
				phi += phiStep;
			}
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

	// add head circle face
	if (isHead)
	{
		for (int i = 0; i < slices*2 - 2; ++i)
		{
			*(indices++) = numRingVerts * 2 + i + 1;
			*(indices++) = numRingVerts * 2 + 0;
			*(indices++) = numRingVerts * 2 + i + 2;
		}

		for (int i = 0; i < slices*2 - 2; ++i)
		{
			*(indices++) = numRingVerts * 3;
			*(indices++) = numRingVerts * 3 + i + 1;
			*(indices++) = numRingVerts * 3 + i + 2;
		}
	}
}


// generate cylinder vertex
// two circle : Y-Z plane
// radius: Y-Z axis
// height: x axis
void cCylinderShape::GenerateConeMesh2(const int slices, const float radius, const float height
	, Vector3* positions, WORD* indices, int offset
	, const bool isHead //= true
)
{
	// generate vertices
	const float phiStep = (MATH_PI * 2.f) / (slices * 2);
	float phi = 0.0f;

	// generate two rings of vertices for the cone ends
	for (int i = 0; i < 2; ++i)
	{
		for (int x = 0; x <= slices * 2; ++x)
		{
			*positions++ = Vector3( (i==0)? height/2.f : -height/2.f
				, (float)cos(phi) * radius, (float)sin(phi) * radius);

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

	// add head face
	if (isHead)
	{
		for (int i = 0; i < slices * 2 - 2; ++i)
		{
			*(indices++) = i + 1;
			*(indices++) = 0;
			*(indices++) = i + 2;
		}

		for (int i = 0; i < slices * 2 - 2; ++i)
		{
			*(indices++) = numRingVerts;
			*(indices++) = numRingVerts + i + 1;
			*(indices++) = numRingVerts + i + 2;
		}
	}
}
