
#include "stdafx.h"
#include "quad.h"


using namespace graphic;


cQuad::cQuad()
	: cNode2(common::GenerateId(), "quad", eNodeType::MODEL)
	, m_texture(NULL)
{
	//m_material.InitWhite();
	//m_material.InitRed();
}

cQuad::cQuad(cRenderer &renderer, const float width, const float height,
	const Vector3 &pos
	, const int vtxType //= (eVertexType::POSITION | eVertexType::TEXTURE)
	, const StrPath &textureFileName // = " "
)
{
	Create(renderer, width, height, pos, vtxType, textureFileName);
}


cQuad::~cQuad()
{
}


// Äõµå¸¦ ÃÊ±âÈ­ ÇÑ´Ù.
// width, height : Äõµå Å©±â
// pos : Äõµå À§Ä¡
bool cQuad::Create(cRenderer &renderer, const float width, const float height,
	const Vector3 &pos 
	, const int vtxType //= (eVertexType::POSITION | eVertexType::TEXTURE)
	, const StrPath &textureFileName // = " "
	)
{
	m_shape.Create(renderer, vtxType);

	m_transform.pos = pos;
	m_transform.scale = Vector3(width, 0, height);

	return true;
}


bool cQuad::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer);

	if (m_texture)
		m_texture->Bind(renderer, 0);

	m_shape.Render(renderer);

	return true;
}
