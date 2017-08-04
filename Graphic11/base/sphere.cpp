#include "stdafx.h"
#include "sphere.h"


using namespace graphic;


cSphere::cSphere() 
	: cNode2(common::GenerateId(), "sphere", eNodeType::MODEL)
	 , m_radius(0)
{
}

cSphere::cSphere(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
)
	: cNode2(common::GenerateId(), "sphere", eNodeType::MODEL)
	, m_radius(0)
{
	Create(renderer, radius, stacks, slices, vtxType);
	m_mtrl.InitWhite();
}

cSphere::~cSphere()
{
}


void cSphere::Create(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
)
{
	m_radius = radius;
	m_shape.Create(renderer, radius, stacks, slices, vtxType);
}


bool cSphere::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	m_shape.Render(renderer);
	__super::Render(renderer, parentTm, flags);
	return true;
}
