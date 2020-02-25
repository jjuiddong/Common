
#include "stdafx.h"
#include "line2dlist.h"

using namespace graphic;


cLine2DList::cLine2DList(
	const uint reserve //= 64
)
	: cNode(common::GenerateId(), "line2d", eNodeType::MODEL)
{
	m_lines.reserve(reserve);
}

cLine2DList::~cLine2DList()
{
}


bool cLine2DList::Create(cRenderer &renderer)
{
	m_shape.Create2(renderer, eVertexType::POSITION_RHW);
	m_vtxType = eVertexType::POSITION_RHW;
	return true;
}


bool cLine2DList::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique("Unlit_Instancing");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	UINT numVp = 1;
	D3D11_VIEWPORT vp;
	renderer.GetDevContext()->RSGetViewports(&numVp, &vp);

	// precompute view, projection matrix
	// because rhw shader no calc view, projection matrix
	Matrix44 proj;
	proj.SetProjectionScreen(vp.Width, vp.Height, 0, 1);
	const XMMATRIX projTm = proj.GetMatrixXM();

	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial.Update(renderer, 2);

	CommonStates state(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(state.CullNone());
	renderer.GetDevContext()->OMSetDepthStencilState(state.DepthNone(), 0);

	const uint maxInstancing = ARRAYSIZE(renderer.m_cbInstancing.m_v->worlds);
	
	uint cnt = 0;
	while (cnt < m_lines.size())
	{
		uint i = 0;
		for (; ((i + cnt) < m_lines.size()) && (i < maxInstancing); ++i)
		{
			const sLine &line = m_lines[i + cnt];
			const Vector4 diffuse = line.color.GetColor();

			renderer.m_cbInstancing.m_v->worlds[i] 
				= XMMatrixTranspose(line.mat.GetMatrixXM() * projTm);
			renderer.m_cbInstancing.m_v->diffuses[i] = 
				XMVectorSet(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
		}
		cnt += i;

		renderer.m_cbInstancing.Update(renderer, 3);
		m_shape.RenderInstancing(renderer, i);
	}

	renderer.GetDevContext()->OMSetDepthStencilState(state.DepthDefault(), 0);
	renderer.GetDevContext()->RSSetState(state.CullCounterClockwise());

	return true;
}


void cLine2DList::AddLine(const Vector2 &p0, const Vector2 &p1, const float width
	, const cColor &color //= cColor::BLACK
)
{
	Vector2 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	Transform tfm;
	Vector2 center = (p1 + p0) / 2.f;
	tfm.pos = Vector3(center.x, center.y, 0);
	tfm.scale = Vector3(width, len / 2.f, 1);

	Quaternion q;
	q.SetRotationArc(Vector3(0, 1, 0), Vector3(v.x, v.y, 0), Vector3(0, 0, 1));
	tfm.rot = q;

	sLine line;
	line.mat = tfm.GetMatrix();
	line.color = color;
	m_lines.push_back(line);
}


void cLine2DList::ClearLine()
{
	m_lines.clear();
}
