
#include "stdafx.h"
#include "viewport.h"

using namespace graphic;

cViewport::cViewport()
{
}

cViewport::cViewport(const float  x, const float  y, const float  width, const float  height,
	const float minZ, const float maxZ)
{
	Create(x, y, width, height, minZ, maxZ);
}

cViewport::~cViewport()
{
}


bool cViewport::Create(
	const float x //=0
	, const float y //=0
	, const float width //=1024
	, const float height //=768
	, const float minZ //=1
	, const float maxZ //=10000
)
{
	m_vp.TopLeftX = x;
	m_vp.TopLeftY = y;
	m_vp.Width = width;
	m_vp.Height = height;
	m_vp.MinDepth = minZ;
	m_vp.MaxDepth = maxZ;
	return true;
}


bool cViewport::Bind(cRenderer &renderer)
{
	renderer.GetDevContext()->RSSetViewports(1, &m_vp);
	return true;
}


float cViewport::GetX() const {
	return m_vp.TopLeftX;
}
float cViewport::GetY() const {
	return m_vp.TopLeftY;
}
float cViewport::GetMinZ() const {
	return m_vp.MinDepth;
}
float cViewport::GetMaxZ() const {
	return m_vp.MaxDepth;
}
float cViewport::GetWidth() const {
	return m_vp.Width;
}
float cViewport::GetHeight() const {
	return m_vp.Height;
}
