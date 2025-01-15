
#include "stdafx.h"
#include "dummy.h"

using namespace graphic;


cDummy::cDummy()
	: cNode(common::GenerateId(), "dummy", eNodeType::MODEL)
{
	m_renderFlags |= eRenderFlag::SHADOW;
	m_subType = eSubType::CUBE;
}


cNode* cDummy::Clone(cRenderer &renderer) const
{
	cDummy*clone = new cDummy();
	return clone;
}
