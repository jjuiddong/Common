
#include "stdafx.h"
#include "cylindershape.h"

using namespace graphic;


cCylinderShape::cCylinderShape()
{
}

cCylinderShape::~cCylinderShape()
{
}


bool cCylinderShape::Create(cRenderer &renderer, const float radius
	, const float height
	, const int slices
	, const int vtxType // = (eVertexType::POSITION | eVertexType::NORMAL| eVertexType::COLOR)
	, const cColor &color // = cColor::BLACK
	, const bool isHead //= true
	, const eCylinderType type //= eCylinderType::AxisX
)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return false; // already exist

	m_radius = radius;
	m_height = height;

	const int sideVtxCnt = slices * 2;
	const int sideIdxCnt = slices * 6;
	const int circleVtxCnt = isHead ? (slices * 2) + 2: 0;
	const int circleIdxCnt = isHead ? (slices * 3) * 2 : 0;

	vector<Vector3> cylinderVtxs(sideVtxCnt + circleVtxCnt);
	vector<unsigned short> cylinderIdxs(sideIdxCnt + circleIdxCnt);
	GenerateCylinderMesh(slices, radius, height, &cylinderVtxs[0], &cylinderIdxs[0], isHead, type);

	const int vtxCount = sideVtxCnt + circleVtxCnt;
	const int faceCount = sideIdxCnt / 3 + circleIdxCnt / 3;
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

	// side face
	for (int i = 0; i < sideVtxCnt; ++i)
	{
		Vector3 p = cylinderVtxs[i];
		Vector3 n = cylinderVtxs[i];
		switch (type)
		{
		case eCylinderType::AxisX: n.x = 0.f; break;
		case eCylinderType::AxisY: n.y = 0.f; break;
		case eCylinderType::AxisZ: n.z = 0.f; break;
		}

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

	// head circle vertex?
	if (isHead)
	{
		// head face vertex
		const int headCircleVtxCnt = (slices * 2); // no center point
		for (int i = 0; i < headCircleVtxCnt; ++i)
		{
			const int s = (i < slices)? 0 : 1;
			const int idx = sideVtxCnt + i;
			Vector3 p = cylinderVtxs[idx];

			// x, -x axis normal
			Vector3 n;
			switch (type)
			{
			case eCylinderType::AxisX: n = (s == 0) ? Vector3(1, 0, 0) : Vector3(-1, 0, 0); break;
			case eCylinderType::AxisY: n = (s == 0) ? Vector3(0, 1, 0) : Vector3(0, -1, 0); break;
			case eCylinderType::AxisZ: n = (s == 0) ? Vector3(0, 0, 1) : Vector3(0, 0, -1); break;
			}

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

		// head face center point (top/bottom 2-point)
		const int headCircleCenterVtxIdx = (slices * 4);
		for (int i = 0; i < 2; ++i)
		{
			const int idx = headCircleCenterVtxIdx + i;
			Vector3 p = cylinderVtxs[idx];

			Vector3 n;
			switch (type)
			{
			case eCylinderType::AxisX: n = (i == 0) ? Vector3(1, 0, 0) : Vector3(-1, 0, 0); break;
			case eCylinderType::AxisY: n = (i == 0) ? Vector3(0, 1, 0) : Vector3(0, -1, 0); break;
			case eCylinderType::AxisZ: n = (i == 0) ? Vector3(0, 0, 1) : Vector3(0, 0, -1); break;
			}

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

	if (pIndices)
	{
		for (int i = 0; i < sideIdxCnt + circleIdxCnt; ++i)
			pIndices[i] = cylinderIdxs[i];
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


// X-Z Plane
//void cCylinderShape::GenerateConeMesh(int slices, Vector3* positions, WORD* indices, int offset
//	, const bool isHead //= true
//)
//{
//	// generate vertices
//	const float phiStep = (MATH_PI * 2.f) / (slices * 2);
//	float phi = 0.0f;
//
//	// generate two rings of vertices for the cone ends
//	for (int i = 0; i < 2; ++i)
//	{
//		for (int x = 0; x <= slices * 2; ++x)
//		{
//			const Vector3 p((float)cos(phi), 0.0f, (float)sin(phi));
//			*(positions++) = p;
//			phi += phiStep;
//		}
//	}
//
//	if (isHead)
//	{
//		// generate two rings of vertices for the circle head
//		for (int i = 0; i < 2; ++i)
//		{
//			for (int x = 0; x <= slices * 2; ++x)
//			{
//				const Vector3 p((float)cos(phi), 0.0f, (float)sin(phi));
//				*(positions++) = p;
//				phi += phiStep;
//			}
//		}
//	}
//
//	const int numRingQuads = 2 * slices;
//	const int numRingVerts = 2 * slices + 1;
//
//	// add faces
//	for (int i = 0; i < numRingQuads; ++i)
//	{
//		// add a quad
//		*(indices++) = offset + i;
//		*(indices++) = offset + numRingVerts + i + 1;
//		*(indices++) = offset + numRingVerts + i;
//
//		*(indices++) = offset + numRingVerts + i + 1;
//		*(indices++) = offset + i;
//		*(indices++) = offset + i + 1;
//	}
//
//	// add head circle face
//	if (isHead)
//	{
//		for (int i = 0; i < slices*2 - 2; ++i)
//		{
//			*(indices++) = numRingVerts * 2 + i + 1;
//			*(indices++) = numRingVerts * 2 + 0;
//			*(indices++) = numRingVerts * 2 + i + 2;
//		}
//
//		for (int i = 0; i < slices*2 - 2; ++i)
//		{
//			*(indices++) = numRingVerts * 3;
//			*(indices++) = numRingVerts * 3 + i + 1;
//			*(indices++) = numRingVerts * 3 + i + 2;
//		}
//	}
//}


// generate cylinder vertex
// two circle : Y-Z plane
// radius: Y-Z axis
// height: x axis
//void cCylinderShape::GenerateConeMesh2(const int slices, const float radius, const float height
//	, Vector3* positions, WORD* indices, int offset
//	, const bool isHead //= true
//)
//{
//	// generate vertices
//	const float phiStep = (MATH_PI * 2.f) / (slices * 2);
//	float phi = 0.0f;
//
//	// generate two rings of vertices for the cone ends
//	for (int i = 0; i < 2; ++i)
//	{
//		for (int x = 0; x <= slices * 2; ++x)
//		{
//			*positions++ = Vector3( (i==0)? height/2.f : -height/2.f
//				, (float)cos(phi) * radius, (float)sin(phi) * radius);
//
//			phi += phiStep;
//		}
//	}
//
//	const int numRingQuads = 2 * slices;
//	const int numRingVerts = 2 * slices + 1;
//
//	// add faces
//	for (int i = 0; i < numRingQuads; ++i)
//	{
//		// add a quad
//		*(indices++) = offset + i;
//		*(indices++) = offset + numRingVerts + i + 1;
//		*(indices++) = offset + numRingVerts + i;
//
//		*(indices++) = offset + numRingVerts + i + 1;
//		*(indices++) = offset + i;
//		*(indices++) = offset + i + 1;
//	}
//
//	// add head face
//	if (isHead)
//	{
//		for (int i = 0; i < slices * 2 - 2; ++i)
//		{
//			*(indices++) = i + 1;
//			*(indices++) = 0;
//			*(indices++) = i + 2;
//		}
//
//		for (int i = 0; i < slices * 2 - 2; ++i)
//		{
//			*(indices++) = numRingVerts;
//			*(indices++) = numRingVerts + i + 1;
//			*(indices++) = numRingVerts + i + 2;
//		}
//	}
//}


// generate cylinder vertex
// radius: cylinder radius
// height: cylinder height
void cCylinderShape::GenerateCylinderMesh(const int slices
	, const float radius, const float height
	, Vector3* positions, WORD* indices
	, const bool isHead //= true
	, const eCylinderType type //= eCylinderType::AxisX
)
{
	// circle for side face vertex
	for (int i = 0; i < 2; ++i)
	{
		const float step = (MATH_PI * 2.f) / slices;
		float angle = 0.0f;
		for (int x = 0; x < slices; ++x)
		{
			Vector3 p;
			switch (type)
			{
			case eCylinderType::AxisX:
				p = Vector3((i == 0) ? height / 2.f : -height / 2.f
					, (float)cos(angle) * radius
					, (float)sin(angle) * radius);
				break;
			case eCylinderType::AxisY:
				p = Vector3((float)cos(angle) * radius
					, (i == 0) ? height / 2.f : -height / 2.f
					, (float)sin(angle) * radius);
				break;
			case eCylinderType::AxisZ:
				p = Vector3((float)cos(angle) * radius
					, (float)sin(angle) * radius
					, (i == 0) ? height / 2.f : -height / 2.f);
				break;
			}
			*positions++ = p;
			angle += step;
		}
	}

	if (isHead)
	{
		// circle for head face vertex
		for (int i = 0; i < 2; ++i)
		{
			const float step = (MATH_PI * 2.f) / slices;
			float angle = 0.0f;
			for (int x = 0; x < slices; ++x)
			{
				Vector3 p;
				switch (type)
				{
				case eCylinderType::AxisX:
					p = Vector3((i == 0) ? height / 2.f : -height / 2.f
						, (float)cos(angle) * radius
						, (float)sin(angle) * radius);
					break;
				case eCylinderType::AxisY:
					p = Vector3((float)cos(angle) * radius
						, (i == 0) ? height / 2.f : -height / 2.f
						, (float)sin(angle) * radius);
					break;
				case eCylinderType::AxisZ:
					p = Vector3((float)cos(angle) * radius
						, (float)sin(angle) * radius
						, (i == 0) ? height / 2.f : -height / 2.f);
					break;
				}
				*positions++ = p;
				angle += step;
			}
		}

		Vector3 top, bottom;
		switch (type)
		{
		case eCylinderType::AxisX:
			top = Vector3(height / 2.f, 0.f, 0.f);
			bottom = Vector3(-height / 2.f, 0.f, 0.f);
			break;
		case eCylinderType::AxisY:
			top = Vector3(0.f, height / 2.f, 0.f);
			bottom = Vector3(0.f, -height / 2.f, 0.f);
			break;
		case eCylinderType::AxisZ:
			top = Vector3(0.f, 0.f, height / 2.f);
			bottom = Vector3(0.f, 0.f, -height / 2.f);
			break;
		}
		*positions++ = top;
		*positions++ = bottom;
	}

	if (indices)
	{
		// add side faces
		for (int i = 0; i < slices; ++i)
		{
			// add a quad
			*(indices++) = i;
			*(indices++) = slices + i;
			*(indices++) = slices + ((i + 1) % slices);

			*(indices++) = slices + ((i + 1) % slices);
			*(indices++) = (i + 1) % slices;
			*(indices++) = i;
		}

		// add head face
		if (isHead)
		{
			const int headTopVtxIdx = slices * 2;
			const int headBottomVtxIdx = slices * 2 + slices;
			const int topIdx = headTopVtxIdx + (slices * 2);
			const int bottomIdx = headTopVtxIdx + (slices * 2) + 1;

			for (int i = 0; i < slices; ++i)
			{
				*(indices++) = topIdx;
				*(indices++) = headTopVtxIdx + i;
				*(indices++) = headTopVtxIdx + (i + 1) % slices;
			}

			for (int i = 0; i < slices; ++i)
			{
				*(indices++) = bottomIdx;
				*(indices++) = headBottomVtxIdx + (i + 1) % slices;
				*(indices++) = headBottomVtxIdx + i;
			}
		}
	}
}
