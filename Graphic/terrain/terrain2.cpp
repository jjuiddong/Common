
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


bool cTerrain2::Create(cRenderer &renderer, const sRectf &rect)
{
	m_frustum.Create(renderer, GetMainCamera()->GetViewProjectionMatrix());
	m_frustum.m_fullCheck = true;

	m_light.Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));

	const Vector2 center = rect.Center();
	const Vector3 lightLookat = Vector3(center.x, 0, center.y);
	const Vector3 lightPos = Vector3(1, -1, 1).Normal() * -400.f + lightLookat;
	m_light.SetPosition(lightPos);
	m_light.SetDirection((lightLookat - lightPos).Normal());

	m_shadowMap.Create(renderer, 1024, 1024);

	const Vector3 p0 = lightPos;// Vector3(center.x, 20, center.y);
	const Vector3 p1 = (lightLookat - lightPos).Normal() * 3 + p0;
	m_dbgLight.Create(renderer, p0, p1, 0.5F);

	Matrix44 view, proj, tt;
	m_light.GetShadowMatrix(view, proj, tt);
	m_dbgLightFrustum.Create(renderer, view * proj);
	m_dbgPlane.SetLine(renderer, Vector3(0, 0, 0), Vector3(0, 30, 0), 0.2f);

	return true;
}


void cTerrain2::Update(cRenderer &renderer, const float deltaSeconds)
{
	for (auto &p : m_tiles)
		p->Update(renderer, deltaSeconds);
}


void cTerrain2::PreRender(cRenderer &renderer)
{
	for (auto &p : m_tiles)
		p->PreRender(renderer, m_shadowMap);
}


void cTerrain2::Render(cRenderer &renderer)
{
	UpdateShader(renderer);
	for (auto &p : m_tiles)
		p->Render(renderer);

	//m_dbgLight.Render(renderer);
	//m_dbgLightFrustum.Render(renderer);
	m_dbgPlane.Render(renderer);
	m_frustum.RenderShader(renderer);
}


void cTerrain2::UpdateShader(cRenderer &renderer)
{
	RET(m_tiles.empty());

	cShader *shader = m_tiles[0]->m_ground.m_shader;
	GetMainCamera()->Bind(*shader);
	GetMainLight().Bind(*shader);
	//m_shader->SetVector("g_vEyePos", GetMainCamera()->GetEyePos());

	GetMainCamera()->Bind(*m_frustum.m_shader);
}


void cTerrain2::CullingTest(
	cRenderer &renderer
	, cCamera &camera
	, const bool isModel //= true
)
{
	m_frustum.SetFrustum(renderer, camera.GetViewProjectionMatrix());

	for (auto &p : m_tiles)
		p->CullingTest(m_frustum, isModel);

	Vector3 orig, dir;
	const int x = camera.m_width / 2;
	const int y = (int)(camera.m_height * 0.8f);
	camera.GetRay(x, y, orig, dir);
	Plane ground(Vector3(0, 1, 0), 0);
	Vector3 pos = ground.Pick(orig, dir);
	m_dbgPlane.SetLine(renderer, pos, pos + Vector3(0, 1, 0) * 10, 0.2f);
}


bool cTerrain2::AddTile(cTile *tile)
{
	auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
	if (m_tiles.end() != it)
		return false; // Already Exist

	m_tiles.push_back(tile);
	m_tilemap[tile->m_name] = tile;
	return true;
}


cTile* cTerrain2::FindTile(const string &name)
{
	auto it = m_tilemap.find(name);
	if (it == m_tilemap.end())
		return NULL;
	return it->second;
}


bool cTerrain2::RemoveTile(cTile *tile)
{
	auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
	if (m_tiles.end() == it)
		return false; // Not Exist

	common::popvector2(m_tiles, tile);
	m_tilemap.erase(tile->m_name);
	return true;
}


void cTerrain2::SetDbgRendering(const bool isRender)
{
	for (auto &p : m_tiles)
		p->m_isDbgRender = isRender;
}


void cTerrain2::LostDevice()
{
	for (auto &p : m_tiles)
		p->LostDevice();
	m_shadowMap.LostDevice();
}


void cTerrain2::ResetDevice(cRenderer &renderer)
{
	for (auto &p : m_tiles)
		p->ResetDevice(renderer);
	m_shadowMap.ResetDevice(renderer);
}


void cTerrain2::Clear()
{
	for (auto &p : m_tiles)
		delete p;
	m_tiles.clear();
}
