
#include "stdafx.h"
#include "text3d3.h"


using namespace graphic;

cText3d3::cText3d3()
	: cNode(common::GenerateId(), "text", eNodeType::TEXT)
	, m_isDepthNone(false)
{
}

cText3d3::~cText3d3()
{
}


bool cText3d3::Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type,
	const int width, const int height
	, const int textWidth // = 256
	, const int textHeight // = 32
	, const float dynScaleMin //= 0.5f
	, const float dynScaleMax //= 200.5f
	, const float dynScaleAlpha //= 1.f
)
{
	if (!m_texture.Create(renderer, textWidth, textHeight, DXGI_FORMAT_R8G8B8A8_UNORM))
		return false;

	if (!m_quad.Create(renderer, type, (float)width, (float)height, Vector3(0, 0, 0)
		, "", true, dynScaleMin, dynScaleMax, dynScaleAlpha))
		return false;

	m_originalScale = m_quad.m_transform.scale;
	m_quad.m_texture = &m_texture;
	m_boundingBox.SetBoundingBox(Vector3(0,0,0), Vector3((float)width, (float)height, 0), Quaternion());
	CalcBoundingSphere();

	return true;
}


// if Render Text return true
// reuse Text return false
bool cText3d3::SetTextRect(
	cRenderer &renderer
	, const Transform &tm
	, const wchar_t *text
	, const cColor &color
	, const cColor &outlineColor
	, const BILLBOARD_TYPE::TYPE type
	)
{
	m_transform = tm;
	m_quad.m_type = type;
	m_quad.m_transform.pos = tm.pos;
	m_quad.m_transform.rot = tm.rot;

	// 텍스트 정보, 칼라가 바뀌면, 텍스쳐에 다시 그린다.
	if ((m_color != color) || (m_text != text))
	{
		m_color = color;

		Vector2 textSize;
		if (m_texture.DrawText(renderer, text, color, outlineColor, textSize))
		{
			// 텍스트를 Quad 가운데 출력하게 한다. (X축만 해당)
			const float u = textSize.x / (float)m_texture.m_imageInfo.Width;
			m_quad.m_shape.SetUV(renderer, Vector2(0, 0), Vector2(u, 0), Vector2(0, 1), Vector2(u, 1));
			m_quad.m_transform.scale = Vector3(
				u * m_originalScale.x * tm.scale.x
				, m_originalScale.y * tm.scale.y
				, m_originalScale.z * tm.scale.z);
		}

		m_text = text;
		return true;
	}

	return false;
}


bool cText3d3::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	if (m_isDepthNone)
	{
		CommonStates state(renderer.GetDevice());
		renderer.GetDevContext()->OMSetDepthStencilState(state.DepthNone(), 0);
		m_quad.Render(renderer);
		renderer.GetDevContext()->OMSetDepthStencilState(state.DepthDefault(), 0);
	}
	else
	{
		m_quad.Render(renderer);
	}
	
	return __super::Render(renderer, parentTm, flags);
}
