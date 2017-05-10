
#include "stdafx.h"
#include "pickmanager.h"

using namespace graphic;


cPickManager::cPickManager()
	: m_mode(ePickMode::SINGLE)
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


bool cPickManager::Update(const float deltaSeconds, const POINT &mousePt)
{
	Vector3 orig, dir;
	cMainCamera::Get()->GetRay(mousePt.x, mousePt.y, orig, dir);

	if (ePickMode::SINGLE == m_mode)
	{
		vector<iPickable*> objs;
		for (auto &obj : m_objects)
		{
			if (obj->IsPickEnable())
				if (obj->Pick(orig, dir))
					objs.push_back(obj);
		}

		RETV(objs.empty(), false);

		if (objs.size() == 1)
		{
			objs[0]->OnPicking(); // Trigger Event
			return true;
		}

		vector<float> lens;
		for (auto &obj : objs)
		{
			float d = 0;
			if (obj->Pick2(orig, dir, &d))
				lens.push_back(d);
		}

		RETV(lens.empty(), false);

		// Find Most Nearest object
		float distance = lens[0];
		int idx = 0;
		for (u_int i = 1; i < lens.size(); ++i)
		{
			if (distance > lens[i])
			{
				idx = i;
			}
		}

		objs[idx]->OnPicking(); // Trigger Event
	}
	else
	{
		for (auto &obj : m_objects)
		{
			if (obj->IsPickEnable())
				if (obj->Pick(orig, dir))
					obj->OnPicking(); // Trigger Event
		}
	}

	return true;
}


void cPickManager::Clear()
{
	m_objects.clear();
}


void cPickManager::SetMode(const ePickMode::Enum mode)
{
	m_mode = mode;
}
