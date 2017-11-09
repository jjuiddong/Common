
#include "stdafx.h"
#include "pickmanager.h"

using namespace graphic;


cPickManager::cPickManager()
	: m_mode(ePickMode::SINGLE)
	, m_offset({ 0,0 })
{
}

cPickManager::~cPickManager()
{
}


bool cPickManager::Add(cNode *node)
{
	auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
	if (m_nodes.end() != it)
		return false; // already exist

	m_nodes.push_back(node);
	return true;
}


bool cPickManager::Remove(cNode *node)
{
	common::popvector2(m_nodes, node);
	return true;
}


bool cPickManager::Pick(const float deltaSeconds, const POINT &mousePt, const ePickState::Enum state)
{
	RETV(m_nodes.empty(), false);

	const Ray ray = GetMainCamera().GetRay(mousePt.x + m_offset.x, mousePt.y + m_offset.y);
	const XMMATRIX mZoom = GetMainCamera().GetZoomMatrix().GetMatrixXM();

	if (ePickMode::SINGLE == m_mode)
	{
		float lens[32];
		cNode *nodes[32];
		int cnt = 0;

		for (auto &node : m_nodes)
		{
			if (node->IsOpFlag(eOpFlag::PICK) && (cnt < 32))
			{
				const XMMATRIX parentTm = node->GetParentWorldMatrix().GetMatrixXM();
				if (node->Picking(ray, node->m_type, parentTm*mZoom))
				{
					nodes[cnt] = node;
					lens[cnt] = node->GetWorldTransform().pos.Distance(ray.orig);
					++cnt;
				}
			}
		}

		RETV(0==cnt, false);

		if (cnt == 1)
		{
			nodes[0]->OnPicking(state); // Trigger Event
			BoradcastPickEvent(nodes[0], state);
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

		nodes[idx]->OnPicking(state); // Trigger Event
		BoradcastPickEvent(nodes[idx], state);
	}
	else
	{
		for (auto &node : m_nodes)
		{
			if (node->IsOpFlag(eOpFlag::PICK))
			{
				const XMMATRIX parentTm = node->GetParentWorldMatrix().GetMatrixXM();
				if (node->Picking(ray, node->m_type, parentTm*mZoom))
				{
					node->OnPicking(state); // Trigger Event
					BoradcastPickEvent(node, state);
				}
			}
		}
	}

	return true;
}


void cPickManager::Clear()
{
	m_nodes.clear();
	m_listeners.clear();
}


void cPickManager::SetMode(const ePickMode::Enum mode)
{
	m_mode = mode;
}


bool cPickManager::AddListener(iPickListener *listener)
{
	auto it = std::find(m_listeners.begin(), m_listeners.end(), listener);
	if (m_listeners.end() != it)
		return false; // already exist

	m_listeners.push_back(listener);
	return true;
}


bool cPickManager::RemoveListener(iPickListener *listener)
{
	auto it = std::find(m_listeners.begin(), m_listeners.end(), listener);
	if (m_listeners.end() == it)
		return false; // not exist

	common::popvector2(m_listeners, listener);
	return true;
}


void cPickManager::BoradcastPickEvent(cNode *node, const ePickState::Enum state)
{
	RET(m_listeners.empty());

	for (auto &p : m_listeners)
		p->OnPickEvent(node, state);
}
