
#include "stdafx.h"
#include "pickmanager.h"

using namespace graphic;


cPickManager::cPickManager()
{
}

cPickManager::~cPickManager()
{
}


bool cPickManager::Add(iPickable *obj)
{
	auto it = std::find(m_objects.begin(), m_objects.end(), obj);
	if (m_objects.end() != it)
		return false; // already exist

	m_objects.push_back(obj);
	return true;
}


bool cPickManager::Remove(iPickable *obj)
{
	common::popvector2(m_objects, obj);
	return true;
}


void cPickManager::Update(const float deltaSeconds)
{
	Vector3 orig, dir;
	cMainCamera::Get()->GetRay(cInputManager::Get()->m_mousePt.x, cInputManager::Get()->m_mousePt.y, orig, dir);

	for (auto &obj : m_objects)
	{
		if (obj->Pick(orig, dir))
			obj->OnPicking(); // Trigger Event
	}
}


void cPickManager::Clear()
{
	m_objects.clear();
}
