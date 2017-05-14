
#include "stdafx.h"
#include "terrain2.h"
#include "tile.h"


using namespace graphic;


cTerrain2::cTerrain2()
{
}

cTerrain2::~cTerrain2()
{
	Clear();
}


void cTerrain2::Update(cRenderer &renderer, const float deltaSeconds)
{
	for (auto &p : m_tiles)
		p->Update(renderer, deltaSeconds);
}


void cTerrain2::PreRender(cRenderer &renderer)
{
	for (auto &p : m_tiles)
		p->PreRender(renderer);
}


void cTerrain2::Render(cRenderer &renderer)
{
	for (auto &p : m_tiles)
		p->Render(renderer);
}


bool cTerrain2::AddTile(cTile *tile)
{
	auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
	if (m_tiles.end() != it)
		return false; // Already Exist

	m_tiles.push_back(tile);
	return true;
}


bool cTerrain2::RemoveTile(cTile *tile)
{
	auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
	if (m_tiles.end() == it)
		return false; // Not Exist

	common::popvector2(m_tiles, tile);
	return true;
}


void cTerrain2::Clear()
{
	for (auto &p : m_tiles)
		delete p;
	m_tiles.clear();
}
