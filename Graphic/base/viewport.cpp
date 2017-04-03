
#include "stdafx.h"
#include "viewport.h"

using namespace graphic;

cViewport::cViewport()
{
}

cViewport::cViewport(const int x, const int y, const int width, const int height,
	const float minZ, const float maxZ)
{
	Create(x, y, width, height, minZ, maxZ);
}

cViewport::~cViewport()
{
}


bool cViewport::Create(const int x, const int y, const int width, const int height,
	const float minZ, const float maxZ)
//	x = 0, y = 0,
// width = 1024, height = 768,
//	minZ = 1.f, maxZ = 10000.f
{
	m_vp.X = x;
	m_vp.Y = y;
	m_vp.Width = width;
	m_vp.Height = height;
	m_vp.MinZ = minZ;
	m_vp.MaxZ = maxZ;
	return true;
}

