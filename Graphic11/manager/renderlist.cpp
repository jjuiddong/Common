
#include "stdafx.h"
#include "renderlist.h"

using namespace graphic;


cRenderList::cRenderList()
{
}

cRenderList::~cRenderList()
{
	Clear();
}


void cRenderList::Render(cRenderer &renderer)
{
	for (auto &kv : m_renderList)
	{
		sRenderInstancing *inst = kv.second;
		if (inst->tms.empty())
			continue;

		uint start = 0;
		while (start < inst->tms.size())
		{
			// max instancing size 256, depend on shader (common.fx)
			const uint size = min((uint)256, inst->tms.size() - start);
			inst->model->RenderInstancing(renderer, (int)size
				, &inst->tms[start], XMIdentity, inst->flags);
			start += size;
		}
	}

	for (auto &kv : m_renderList)
		kv.second->tms.clear();
}


bool cRenderList::AddRender(cModel *model
	, const Matrix44 &transformTm //= Matrix44::Identity
	, const int flags //= 1
)
{
	sRenderInstancing *inst = NULL;
	auto it = m_renderList.find(model->m_fileName.c_str());
	if (m_renderList.end() == it)
	{
		inst = new sRenderInstancing;
		inst->name = model->m_fileName.c_str();
		inst->model = model;
		inst->flags = flags;
		inst->tms.reserve(32);
		m_renderList.insert({ model->m_fileName.c_str(), inst });
	}
	else
	{
		inst = it->second;
	}

	if (!inst)
		return false;

	inst->flags = flags;
	inst->tms.push_back(transformTm);
	return true;
}


void cRenderList::Clear()
{
	for (auto &kv : m_renderList)
		delete kv.second;
	m_renderList.clear();
}
