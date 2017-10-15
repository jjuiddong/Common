
#include "stdafx.h"
#include "pickmanager.h"

using namespace graphic;


cPickManager::cPickManager()
	: m_mode(ePickMode::SINGLE)
	, m_mainCamera(NULL)
	, m_offset({ 0,0 })
{
}

cPickManager::~cPickManager()
{
}


bool cPickManager::Add(cNode2 *node)
{
	auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
	if (m_nodes.end() != it)
		return false; // already exist

	m_nodes.push_back(node);
	return true;
}


bool cPickManager::Remove(cNode2 *node)
{
	common::popvector2(m_nodes, node);
	return true;
}


bool cPickManager::Pick(const float deltaSeconds, const POINT &mousePt)
{
	RETV(!m_mainCamera, false);

	Vector3 orig, dir;
	m_mainCamera->GetRay(mousePt.x + m_offset.x, mousePt.y + m_offset.y, orig, dir);

	if (ePickMode::SINGLE == m_mode)
	{
		float lens[32];
		cNode2 *nodes[32];
		int cnt = 0;

		for (auto &node : m_nodes)
		{
			if (node->IsOpFlag(eOpFlag::PICK) && (cnt < 32))
			{
				if (node->Picking(orig, dir, node->m_type
					, node->m_parent? node->m_parent->GetWorldTransform().GetMatrixXM() : XMIdentity))
				{
					nodes[cnt] = node;
					lens[cnt] = node->GetWorldTransform().pos.Distance(orig);
					++cnt;
				}
			}
		}

		RETV(0==cnt, false);

		if (cnt == 1)
		{
			nodes[0]->OnPicking(); // Trigger Event
			return true;
		}

		// Find Most Nearest object
		float distance = lens[0];
		int idx = 0;
		for (int i = 1; i < cnt; ++i)
		{
			if (distance > lens[i])
			{
				idx = i;
				distance = lens[i];
			}
		}

		nodes[idx]->OnPicking(); // Trigger Event
	}
	else
	{
		for (auto &node : m_nodes)
		{
			if (node->IsOpFlag(eOpFlag::PICK))
				if (node->Picking(orig, dir, node->m_type))
					node->OnPicking(); // Trigger Event
		}
	}

	return true;
}


void cPickManager::Clear()
{
	m_nodes.clear();
}


void cPickManager::SetMode(const ePickMode::Enum mode)
{
	m_mode = mode;
}
