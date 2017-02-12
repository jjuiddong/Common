
#include "stdafx.h"
#include "model2.h"
#include "mesh2.h"

using namespace graphic;


cModel2::cModel2()
	: m_root(NULL)
{
}

cModel2::~cModel2()
{
	Clear();
}


bool cModel2::Create(cRenderer &renderer, const string &modelName,
	MODEL_TYPE::TYPE type, const bool isLoadShader)
{
	sRawMeshGroup2 *rawMeshes = cResourceManager::Get()->LoadModel2(modelName);
	RETV(!rawMeshes, false);

	Clear();

	m_tmPalette.resize(rawMeshes->nodeCount, Matrix44::Identity);

	m_root = new cNode2();
	m_root->Create(renderer, rawMeshes->root, &m_tmPalette);

	UpdateBoundingBox();

	return true;
}


void cModel2::Render(cRenderer &renderer, const Matrix44 &tm)
// tm = Matrix44::Identity
{
	if (m_root)
		m_root->Render(renderer, tm);
}


bool cModel2::Update(const float deltaSeconds)
{
	if (m_root)
		m_root->Update(deltaSeconds);
	return true;
}


void cModel2::Clear()
{
	SAFE_DELETE(m_root);
}


void cModel2::UpdateBoundingBox() 
{
	RET(!m_root);

	sMinMax mm;

	vector<cNode2*> q;
	q.push_back(m_root);
	while (!q.empty())
	{
		cNode2 *p = q.back();
		q.pop_back();

		for (auto m : p->m_meshes)
		{
			mm.Update(m->m_buffers->m_boundingBox.m_min);
			mm.Update(m->m_buffers->m_boundingBox.m_max);
		}

		for (auto c : p->m_children)
			q.push_back(c);
	}

	m_boundingBox.SetBoundingBox(mm._min, mm._max);
}
