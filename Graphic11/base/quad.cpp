
#include "stdafx.h"
#include "quad.h"

using namespace graphic;


cQuad::cQuad()
	: cNode(common::GenerateId(), "quad", eNodeType::MODEL)
	, m_texture(NULL)
{
}

cQuad::cQuad(cRenderer &renderer, const float width, const float height
	, const Vector3 &pos
	, const int vtxType //= (eVertexType::POSITION | eVertexType::TEXTURE0)
	, const StrPath &textureFileName // = " "
	, const cQuadShape::ePlaneType planeType //= cQuadShape::ePlaneType::XY
)
{
	Create(renderer, width, height, pos, vtxType, textureFileName, false, planeType);
}

cQuad::~cQuad()
{
}


// 쿼드를 초기화 한다.
// width, height : 쿼드 크기, 중점을 기준으로한 폭, 높이 크기
// pos : 쿼드 위치
bool cQuad::Create(cRenderer &renderer, const float width, const float height,
	const Vector3 &pos 
	, const int vtxType //= (eVertexType::POSITION | eVertexType::TEXTURE0)
	, const StrPath &textureFileName // = ""
	, const bool isDynamic // = false
	, const cQuadShape::ePlaneType planeType //= cQuadShape::ePlaneType::XY
)
{
	m_shape.Create(renderer, vtxType, cColor::WHITE, 2, 2, isDynamic, planeType);
	m_vtxType = vtxType;
	m_color = cColor::WHITE;
	m_transform.pos = pos;
	m_transform.scale = (planeType == cQuadShape::ePlaneType::XY)? 
		Vector3(width, height, 0.1f) : Vector3(width, 0.1f, height);
	m_boundingBox.SetBoundingBox(m_transform);

	if (!textureFileName.empty())
	{
		m_texture = cResourceManager::Get()->LoadTextureParallel(renderer, textureFileName);
		cResourceManager::Get()->AddParallelLoader(new cParallelLoader(cParallelLoader::eType::TEXTURE
			, textureFileName, (void**)&m_texture));
	}

	return true;
}


// render
bool cQuad::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	Vector4 diffuse = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = diffuse.GetVectorXM();
	renderer.m_cbMaterial.Update(renderer, 2);

	if (m_texture)
		m_texture->Bind(renderer, 0);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
	m_shape.Render(renderer);
	renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);

	return __super::Render(renderer, parentTm, flags);
}


// instancing render
bool cQuad::RenderInstancing(cRenderer& renderer
	, const int count
	, const Matrix44* transforms
	, const XMMATRIX& parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11* shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	const Str32 technique = m_techniqueName + "_Instancing";
	shader->SetTechnique(technique.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;
	for (int i = 0; i < count; ++i)
	{
		const XMMATRIX tm = transforms[i].GetMatrixXM();
		renderer.m_cbInstancing.m_v->worlds[i] = XMMatrixTranspose(tm * transform);
	}

	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbInstancing.Update(renderer, 3);

	const Vector4 color = m_color.GetColor();
	Vector4 ambient = m_color.GetColor();
	Vector4 diffuse = m_color.GetColor();
	renderer.m_cbMaterial.m_v->ambient = XMVectorSet(ambient.x, ambient.y, ambient.z, ambient.w);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	m_shape.RenderInstancing(renderer, count);

	return __super::RenderInstancing(renderer, count, transforms, parentTm, flags);
}


// instancing render
bool cQuad::RenderInstancing2(cRenderer& renderer
	, const int count
	, const Matrix44* transforms
	, const Vector4* colors
	, const XMMATRIX& parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11* shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	const Str32 technique = m_techniqueName + "_Instancing2";
	shader->SetTechnique(technique.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;
	for (int i = 0; i < count; ++i)
	{
		const XMMATRIX tm = transforms[i].GetMatrixXM();
		const XMVECTOR col = colors[i].GetVectorXM();
		renderer.m_cbInstancing.m_v->worlds[i] = XMMatrixTranspose(tm * transform);
		renderer.m_cbInstancing.m_v->diffuses[i] = col;
	}

	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbInstancing.Update(renderer, 3);

	renderer.m_cbMaterial.m_v->ambient = XMVectorSet(1, 1, 1, 1);
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(1, 1, 1, 1);
	renderer.m_cbMaterial.Update(renderer, 2);

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.NonPremultiplied(), 0, 0xffffffff);
		m_shape.RenderInstancing(renderer, count);
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	}
	else
	{
		m_shape.RenderInstancing(renderer, count);
	}

	return __super::RenderInstancing(renderer, count, transforms, parentTm, flags);
}


//
//// Set UV Position
//// Vector2.xy = {u , v}
//// lt : left top
//// rt : right top
//// lb : left bottom
//// rb : right bottom
//void cQuad::SetUV(cRenderer &renderer, const Vector2 &lt, const Vector2 &rt, const Vector2 &lb, const Vector2 &rb)
//{
//	RET(m_shape.m_vtxBuff.GetVertexCount() <= 0);
//
//	const int posOffset = m_shape.m_vtxLayout.GetOffset("POSITION");
//	const int normOffset = m_shape.m_vtxLayout.GetOffset("NORMAL");
//	const int colorOffset = m_shape.m_vtxLayout.GetOffset("COLOR");
//	const int texOffset = m_shape.m_vtxLayout.GetOffset("TEXCOORD");
//	const int vertexStride = m_shape.m_vtxLayout.GetVertexSize();
//
//	BYTE *vertices = (BYTE*)m_shape.m_vtxBuff.Lock(renderer);
//	RET(!vertices);
//
//	Vector2 *uv0 = (Vector2*)(vertices + (vertexStride * 0) + texOffset);
//	Vector2 *uv1 = (Vector2*)(vertices + (vertexStride * 1) + texOffset);
//	Vector2 *uv2 = (Vector2*)(vertices + (vertexStride * 2) + texOffset);
//	Vector2 *uv3 = (Vector2*)(vertices + (vertexStride * 3) + texOffset);
//
//	// left top
//	*uv0 = lt;
//	// right top
//	*uv1 = rt;
//	// left bottom
//	*uv2 = lb;
//	// right bottom
//	*uv3 = rb;
//
//	m_shape.m_vtxBuff.Unlock(renderer);
//}
