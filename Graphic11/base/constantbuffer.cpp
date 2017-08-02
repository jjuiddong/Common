
#include "stdafx.h"
#include "constantbuffer.h"

using namespace graphic;


cConstantBuffer::cConstantBuffer()
	: m_constantBuffer(NULL)
{
}

cConstantBuffer::~cConstantBuffer()
{
	Clear();
}


bool cConstantBuffer::Create(cRenderer &renderer, const int byteSize)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = byteSize;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	if (FAILED(renderer.GetDevice()->CreateBuffer(&bd, NULL, &m_constantBuffer)))
		return false;

	return true;
}


bool cConstantBuffer::Update(cRenderer &renderer, const void *ptr)
{
	RETV(!m_constantBuffer, false);
	renderer.GetDevContext()->UpdateSubresource(m_constantBuffer, 0, NULL, ptr, 0, 0);
	return true;
}


bool cConstantBuffer::Bind(cRenderer &renderer
	, const int slot //=0
)
{
	RETV(!m_constantBuffer, false);
	renderer.GetDevContext()->VSSetConstantBuffers(slot, 1, &m_constantBuffer);
	renderer.GetDevContext()->PSSetConstantBuffers(slot, 1, &m_constantBuffer);
	return true;
}


bool cConstantBuffer::UpdateAndBind(cRenderer &renderer, const void *ptr)
{
	if (!Update(renderer, ptr))
		return false;
	return Bind(renderer);
}


void cConstantBuffer::Clear()
{
	SAFE_RELEASE(m_constantBuffer);
}
