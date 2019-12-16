
#include "stdafx.h"
#include "sharebuffer.h"


using namespace graphic;

cShareBuffer::cShareBuffer()
	: m_buff(nullptr)
	, m_srv(nullptr)
	, m_uav(nullptr)
{
}

cShareBuffer::~cShareBuffer()
{
	Clear();
}


// temporal code
// https://www.codeproject.com/Articles/42612/DirectX-Compute-Shaders
//
// Create share Buffer
// readable, writable buffer
// very slow buffer, only use temporal access GPU Buffer
//
bool cShareBuffer::Create(cRenderer &renderer, const void *pInitMem
	, const int stride, const int count
	, const bool isCPUWritable //= false
	, const bool isGPUWritable //= false
)
{
	SAFE_RELEASE(m_buff);
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = stride * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = stride;

	if (!isCPUWritable && !isGPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}
	else if (isCPUWritable && !isGPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (!isCPUWritable && isGPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE
			| D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	else if (isCPUWritable && isGPUWritable)
	{
		// error occurred
		// no operation
		return false;
	}

	if (pInitMem)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitMem;
		//InitData.SysMemPitch = stride;
		//InitData.SysMemSlicePitch = 0;
		if (FAILED(renderer.GetDevice()->CreateBuffer(&desc, &InitData, &m_buff)))
		{
			assert(0);
			return false;
		}
	}
	else
	{
		if (FAILED(renderer.GetDevice()->CreateBuffer(&desc, NULL, &m_buff)))
		{
			assert(0);
			return false;
		}
	}

	m_stride = stride;
	m_count = count;

	if (!isGPUWritable) // shader resource?
		if (!CreateShaderResourceView(renderer, desc))
			return false;

	if (isGPUWritable)
		if (!CreateUnorderedAccessView(renderer, desc))
			return false;

	return true;
}


// create buffer from srcBuffer description
bool cShareBuffer::CreateReadBuffer(cRenderer &renderer, const cShareBuffer &src)
{
	RETV2(!src.m_buff, false);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	src.m_buff->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	if (FAILED(renderer.GetDevice()->CreateBuffer(&desc, NULL, &m_buff)))
	{
		assert(0);
		return false;
	}

	return true;
}


bool cShareBuffer::CopyFrom(cRenderer &renderer, const cShareBuffer &src)
{
	RETV2(!src.m_buff, false);
	RETV2(!m_buff, false);
	renderer.GetDevContext()->CopyResource(m_buff, src.m_buff);
	return true;
}


// create shader resouceview
bool cShareBuffer::CreateShaderResourceView(cRenderer &renderer, const D3D11_BUFFER_DESC &desc)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(sdesc));
	sdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	sdesc.BufferEx.FirstElement = 0;

	if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer
		sdesc.Format = DXGI_FORMAT_R32_TYPELESS;
		sdesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		sdesc.BufferEx.NumElements = desc.ByteWidth / 4;
	}
	else
	{
		if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer
			sdesc.Format = DXGI_FORMAT_UNKNOWN;
			sdesc.BufferEx.NumElements = desc.ByteWidth / desc.StructureByteStride;
		}
		else
		{
			return false;
		}
	}

	if (FAILED(renderer.GetDevice()->CreateShaderResourceView(m_buff, &sdesc, &m_srv)))
	{
		assert(0);
		return false;
	}

	return true;
}


// create unordered access view
bool cShareBuffer::CreateUnorderedAccessView(cRenderer &renderer, const D3D11_BUFFER_DESC &desc)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC udesc;
	ZeroMemory(&udesc, sizeof(udesc));
	udesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	udesc.Buffer.FirstElement = 0;

	if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer
		// Format must be DXGI_FORMAT_R32_TYPELESS,
		// when creating Raw Unordered Access View
		udesc.Format = DXGI_FORMAT_R32_TYPELESS;
		udesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		udesc.Buffer.NumElements = desc.ByteWidth / 4;
	}
	else
	{
		if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer
			// Format must be must be DXGI_FORMAT_UNKNOWN, 
			// when creating a View of a Structured Buffer
			udesc.Format = DXGI_FORMAT_UNKNOWN;
			udesc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		}
		else
		{
			return false;
		}
	}

	if (FAILED(renderer.GetDevice()->CreateUnorderedAccessView(m_buff, &udesc, &m_uav)))
	{
		assert(0);
		return false;
	}
	return true;
}


bool cShareBuffer::Bind(cRenderer &renderer
	, const int stage //= 0
)
{
	renderer.GetDevContext()->CSSetShaderResources(stage, 1, &m_srv);
	return true;
}


bool cShareBuffer::BindUnorderedAccessView(cRenderer &renderer
	, const int stage //= 0
)
{
	renderer.GetDevContext()->CSSetUnorderedAccessViews(stage, 1, &m_uav, (UINT*)&m_uav);
	return true;
}


void* cShareBuffer::Lock(cRenderer &renderer
	, const D3D11_MAP flag //= D3D11_MAP_READ
)
{
	assert(m_buff);
	D3D11_MAPPED_SUBRESOURCE res;
	ZeroMemory(&res, sizeof(res));
	HRESULT hr = renderer.GetDevContext()->Map(m_buff, 0, flag, 0, &res);
	if (FAILED(hr))
		return NULL;
	return res.pData;
}


void* cShareBuffer::Lock(cRenderer &renderer, OUT D3D11_MAPPED_SUBRESOURCE &out)
{
	assert(m_buff);
	ZeroMemory(&out, sizeof(out));
	HRESULT hr = renderer.GetDevContext()->Map(m_buff, 0, D3D11_MAP_READ, 0, &out);
	if (FAILED(hr))
		return NULL;
	return out.pData;
}


void cShareBuffer::Unlock(cRenderer &renderer)
{
	assert(m_buff);
	renderer.GetDevContext()->Unmap(m_buff, 0);
}


void cShareBuffer::Clear()
{
	SAFE_RELEASE(m_buff);
	SAFE_RELEASE(m_srv);
	SAFE_RELEASE(m_uav);
}
