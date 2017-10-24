
#include "stdafx.h"
#include "texture.h"


using namespace graphic;


cTexture::cTexture() 
	: m_texSRV(NULL)
	, m_texture(NULL)
	,  m_isReferenceMode(false)
	//, m_customTexture(false)
{
}

cTexture::cTexture(ID3D11ShaderResourceView *srv)
	: m_texSRV(srv)
	, m_isReferenceMode(true)
{
}

cTexture::~cTexture()
{
	Clear();
}


bool cTexture::Create(cRenderer &renderer, const StrPath &fileName)
{
	Clear();

	 m_fileName = ConvertTextureFileName(fileName.c_str());

	if (FAILED(CreateDDSTextureFromFile(renderer.GetDevice(), m_fileName.wstr().c_str(), NULL, &m_texSRV)))
	{
		if (FAILED(CreateWICTextureFromFile(renderer.GetDevice(), m_fileName.wstr().c_str(), NULL, &m_texSRV)))
			return false;
	}

	ID3D11Resource *res;
	m_texSRV->GetResource(&res);

	ID3D11Texture2D *pTextureInterface = 0;
	res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	D3D11_TEXTURE2D_DESC desc;
	pTextureInterface->GetDesc(&desc);
	SAFE_RELEASE(res);
	SAFE_RELEASE(pTextureInterface);

	m_imageInfo = desc;
	return true;
}


bool cTexture::Create(cRenderer &renderer, const int width, const int height
	, const DXGI_FORMAT format //=DXGI_FORMAT_R8G8B8A8_UNORM
)
{
	Clear();

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(renderer.GetDevice()->CreateTexture2D(&desc, NULL, &m_texture)))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC rdesc;
	ZeroMemory(&rdesc, sizeof(rdesc));
	rdesc.Format = format;
	rdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	rdesc.Texture2D.MipLevels = 1;

	if (FAILED(renderer.GetDevice()->CreateShaderResourceView(m_texture, &rdesc, &m_texSRV)))
		return false;

	//D3DLOCKED_RECT lockrect;
	//m_texture->LockRect( 0, &lockrect, NULL, 0 );
	//memset( lockrect.pBits, 0x00, lockrect.Pitch*height );
	//m_texture->UnlockRect( 0 );

	//m_customTexture = true;
	m_imageInfo.Width = width;
	m_imageInfo.Height = height;
	m_imageInfo.Format = format;
	return true;
}


bool cTexture::Create(cRenderer &renderer, const int width, const int height
	, const DXGI_FORMAT format
	, const void *pMem
	, const int pitchLength)
{
	Clear();

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subresource_data;
	ZeroMemory(&subresource_data, sizeof(subresource_data));
	subresource_data.pSysMem = pMem;
	subresource_data.SysMemPitch = pitchLength;
	subresource_data.SysMemSlicePitch = 0;

	if (FAILED(renderer.GetDevice()->CreateTexture2D(&desc, &subresource_data, &m_texture)))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC rdesc;
	ZeroMemory(&rdesc, sizeof(rdesc));
	rdesc.Format = format;
	rdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	rdesc.Texture2D.MipLevels = 1;

	if (FAILED(renderer.GetDevice()->CreateShaderResourceView(m_texture, &rdesc, &m_texSRV)))
		return false;

	//m_customTexture = true;
	m_imageInfo.Width = width;
	m_imageInfo.Height = height;
	m_imageInfo.Format = format;
	return true;
}


void cTexture::Bind(cRenderer &renderer
	, const int stage //= 0
)
{
	renderer.GetDevContext()->PSSetShaderResources(stage, 1, &m_texSRV);
}


void cTexture::Bind(cShader &shader, const Str32 &key)
{
	assert(0);
	//shader.SetTexture(key, *this);
}

void cTexture::Unbind(cRenderer &renderer, const int stage)
{
	ID3D11ShaderResourceView *ns[1] = { NULL };
	renderer.GetDevContext()->PSSetShaderResources(stage, 1, ns);
}


void cTexture::Render2D(cRenderer &renderer)
{
	//renderer.GetDevice()->SetTransform(D3DTS_WORLD, ToDxM(Matrix44::Identity));
	//renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	//renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//renderer.GetDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	////renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	////renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	//renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	//renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	//typedef struct { FLOAT p[4]; FLOAT tu, tv; } TVERTEX;
	//TVERTEX Vertex[4] = {
	//	// x  y  z rhw tu tv
	//	{ 0, 0, 0, 1, 0, 0, },
	//	{ (float)m_imageInfo.Width-1, 0,0, 1, 1, 0, },
	//	{ (float)m_imageInfo.Width-1, (float)m_imageInfo.Height-1, 1, 1, 1, 1},
	//	{ 0, (float)m_imageInfo.Height-1,0, 1, 0, 1, },
	//};
	//renderer.GetDevice()->SetTexture(0, m_texture);
	//renderer.GetDevice()->SetVertexShader(NULL);
	//renderer.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	//renderer.GetDevice()->SetPixelShader(NULL);
	//renderer.GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Vertex, sizeof(TVERTEX));
}


void* cTexture::Lock(cRenderer &renderer, OUT D3D11_MAPPED_SUBRESOURCE &out)
{
	ZeroMemory(&out, sizeof(out));
	HRESULT hr = renderer.GetDevContext()->Map(m_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &out);
	if (FAILED(hr))
		return NULL;
	return out.pData;
}


void cTexture::Unlock(cRenderer &renderer)
{
	renderer.GetDevContext()->Unmap(m_texture, 0);
}


void cTexture::Clear()
{
	m_fileName.clear();

	if (m_isReferenceMode)
	{
		m_texSRV = NULL;
		m_texture = NULL;
	}
	else
	{
		SAFE_RELEASE(m_texSRV);
		SAFE_RELEASE(m_texture);
	}
}


// 텍스쳐를 파일에 저장한다.
bool cTexture::WritePNGFile( const StrPath &fileName )
{
	//if (FAILED(D3DXSaveTextureToFileA(fileName.c_str(), D3DXIFF_PNG, m_texture, NULL)))
	//{
	//	return false;
	//}
	return true;
}


// Render Text String on Texture using GdiPlus
// https://www.codeproject.com/Articles/42529/Outline-Text
bool cTexture::DrawText(cRenderer &renderer, const wchar_t *szText
	, const cColor &color, const cColor &outlineColor
	, Vector2 &textSize)
{
	using namespace Gdiplus;
	Graphics &g = *renderer.m_textMgr.m_graphic.get();

	const int fontSize = 28;
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g.SetPageUnit(UnitPixel);

	FontFamily fontFamily(L"Arial");
	StringFormat strformat;

	TextDesigner::PngOutlineText text;

	const Vector4 tColor = color.GetColor();
	const Vector4 oColor = outlineColor.GetColor();
	text.TextOutline(Color((int)(tColor.x*255), (int)(tColor.y * 255), (int)(tColor.z * 255))
		, Color((int)(oColor.x * 255), (int)(oColor.y * 255), (int)(oColor.z * 255)), 4);
	text.EnableShadow(false);

	float fWidth = 0.0f;
	float fHeight = 0.0f;
	text.MeasureString(&g, &fontFamily, FontStyleBold,
		fontSize, szText, Gdiplus::Point(0, 0), &strformat,
		NULL, NULL, &fWidth, &fHeight);
	textSize.x = fWidth;
	textSize.y = fHeight;

	std::shared_ptr<Gdiplus::Bitmap> pbmp = renderer.m_textMgr.m_textBmp;
	Graphics &clearTxtBmp = *renderer.m_textMgr.m_graphicText.get();
	clearTxtBmp.Clear(Gdiplus::Color(0,255,255,255));
	//clearTxtBmp.Clear(Gdiplus::Color(255, 255, 255, 255));

	text.SetPngImage(pbmp);
	text.DrawString(&g, &fontFamily, FontStyleBold,
		fontSize, szText, Gdiplus::Point(0, 0), &strformat);

	Rect rect1(0, 0, pbmp->GetWidth(), pbmp->GetHeight());
	BitmapData bitmapData;
	memset(&bitmapData, 0, sizeof(bitmapData));
	pbmp->LockBits(
		&rect1,
		ImageLockModeRead,
		PixelFormat32bppARGB,
		&bitmapData);

	const int pitch = abs(bitmapData.Stride);
	UINT* pixels = (UINT*)bitmapData.Scan0;
	if (!pixels)
		return false;

	D3D11_MAPPED_SUBRESOURCE res;
	if (UINT* dpixels = (UINT*)Lock(renderer, res))
	{
		memcpy(dpixels, pixels, bitmapData.Height*pitch);
		Unlock(renderer);
	}

	pbmp->UnlockBits(&bitmapData);

	return true;
}


int cTexture::Width()
{
	return m_imageInfo.Width;
}


int cTexture::Height()
{
	return m_imageInfo.Height;
}


void cTexture::LostDevice()
{
	//RET(!m_texture);
	//
	//if (m_customTexture)
	//	SAFE_RELEASE(m_texture);
}


void cTexture::ResetDevice(cRenderer &renderer)
{
	//RET(!m_customTexture);

	//SAFE_RELEASE(m_texSRV);

	//if (m_fileName.empty())
	//{
	//	//Create(renderer, m_imageInfo.Width, m_imageInfo.Height, m_imageInfo.Format);
	//}
	//else
	//{
	//	Create(renderer, StrPath(m_fileName));
	//}
}


bool cTexture::IsLoaded()
{
	return m_texSRV? true : false;
}


// return true if converting filName
// convert file extention TGA -> DDS
// because, did not load tga format, you must change format tga to dds
StrPath cTexture::ConvertTextureFileName(const char *fileName)
{
	StrPath out(fileName);
	const Str32 ext = out.GetFileExt();
	if ((ext == ".tga") || (ext == ".TGA"))
	{
		const int len = out.size();
		out.m_str[len - 3] = 'd';
		out.m_str[len - 2] = 'd';
		out.m_str[len - 1] = 's';
	}	
	return out;
}
