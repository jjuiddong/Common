
#include "stdafx.h"
#include "dbgboxline.h"

using namespace graphic;


cDbgBoxLine::cDbgBoxLine()
	: m_isUpdate(false)
{
}

cDbgBoxLine::cDbgBoxLine(cRenderer &renderer, const cBoundingBox &bbox
	, const cColor &color //= cColor::BLACK
)
	: m_isUpdate(false)
{
	Create(renderer, bbox, color);
}


void cDbgBoxLine::Create(cRenderer &renderer
	, const cColor &color //= cColor::BLACK
)
{
	m_color = color;
	m_lines.Create(renderer, 12, color);
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0,0,0,1));
}


void cDbgBoxLine::Create(cRenderer &renderer, const cBoundingBox &bbox
	, const cColor &color //= cColor::BLACK
)
{
	m_color = color;
	m_lines.Create(renderer, 12, color);
	SetBox(bbox);
}


void cDbgBoxLine::SetBox(const cBoundingBox &bbox)
{
	m_isUpdate = true;
	m_boundingBox = bbox;
}


void cDbgBoxLine::SetBox(const Transform &tfm)
{
	m_isUpdate = true;
	m_boundingBox.SetBoundingBox(tfm);
}

//
//void cDbgBoxLine::SetBox(cRenderer &renderer, const Vector3 vertices[8]
//	, const cColor &color //= cColor::BLACK
//)
//{
//	m_shape.Create(renderer, vertices, eVertexType::POSITION | eVertexType::COLOR, color);
//	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0, 0, 0, 1));
//}


void cDbgBoxLine::SetBox(cRenderer &renderer, const Vector3 vertices[8]
	, const cColor &color // = cColor::BLACK
)
{
	//m_shape.SetCube(renderer, vertices, color);
	m_isUpdate = true;
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0, 0, 0, 1));
}


void cDbgBoxLine::SetColor(const cColor &color)
{
	m_color = color;
	m_lines.m_color = color;
}


// Render Box using Triangle
void cDbgBoxLine::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	if (m_isUpdate)
	{
		m_isUpdate = false;
		UpdateLine(renderer);
	}
	m_lines.Render(renderer, tm);
}


void cDbgBoxLine::RenderInstancing(cRenderer &renderer
	, const int count
	, const XMMATRIX *transforms
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	// todo: render instancing
}


XMMATRIX cDbgBoxLine::GetTransform() const
{
	return m_boundingBox.GetMatrixXM();
}


// update box line from bounding box
void cDbgBoxLine::UpdateLine(cRenderer& renderer)
{
	// cube box vertex array
	const float s = 1.f;
	Vector3 vtxs[8] = {
		Vector3(-s, s, -s), Vector3(-s, s, s), Vector3(s, s, s), Vector3(s, s, -s),
		Vector3(-s, -s, -s), Vector3(-s, -s, s), Vector3(s, -s, s), Vector3(s, -s, -s),
	};
	const Matrix44 mat = m_boundingBox.GetMatrix();
	for (int i = 0; i < 8; ++i)
		vtxs[i] = vtxs[i] * mat;

	// cube box line index
	const int indices[24] = {
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7,
	};
	m_lines.ClearLines();
	for (int i=0; i < 24; i+=2)
		m_lines.AddLine(renderer, vtxs[indices[i]], vtxs[indices[i + 1]], false);
	m_lines.UpdateBuffer(renderer);
}
