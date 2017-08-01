
#include "stdafx.h"
#include "texture.h"


using namespace graphic;


cTexture::cTexture() 
	//: m_texture(NULL)
	: m_isReferenceMode(false)
	, m_customTexture(false)
{
}

cTexture::~cTexture()
{
	Clear();
}


bool cTexture::Create(cRenderer &renderer, const StrPath &fileName)
{
	Clear();

	m_fileName = fileName;

	if (FAILED(D3DX11CreateShaderResourceViewFromFileA(renderer.GetDevice(), fileName.c_str(), NULL, NULL, &m_texture, NULL)))
		return false;

	// 텍스쳐 사이즈 저장.
	ID3D11Resource *res;
	m_texture->GetResource(&res);

	ID3D11Texture2D *pTextureInterface = 0;
	res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	D3D11_TEXTURE2D_DESC desc;
	pTextureInterface->GetDesc(&desc);
	SAFE_RELEASE(res);
	SAFE_RELEASE(pTextureInterface);

	m_imageInfo = desc;
	return true;
}


bool cTexture::Create(cRenderer &renderer, const int width, const int height, const D3DFORMAT format)
{
	Clear();

	//if (FAILED(renderer.GetDevice()->CreateTexture( width, height, 1, 0, format, 
	//	D3DPOOL_MANAGED, &m_texture, NULL )))
	//	return false;

	//D3DLOCKED_RECT lockrect;
	//m_texture->LockRect( 0, &lockrect, NULL, 0 );
	//memset( lockrect.pBits, 0x00, lockrect.Pitch*height );
	//m_texture->UnlockRect( 0 );

	//m_customTexture = true;
	//m_imageInfo.Width = width;
	//m_imageInfo.Height = height;
	//m_imageInfo.Format = format;
	return true;
}


void cTexture::Bind(cRenderer &renderer
	, const int stage //= 0
)
{
	renderer.GetDevContext()->PSSetShaderResources(stage, 1, &m_texture);

	//renderer.GetDevice()->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//renderer.GetDevice()->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//renderer.GetDevice()->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	//renderer.GetDevice()->SetTexture(stage, m_texture);
}


void cTexture::Bind(cShader &shader, const Str32 &key)
{
	//shader.SetTexture(key, *this);
}

void cTexture::Unbind(cRenderer &renderer, const int stage)
{
	//renderer.GetDevice()->SetTexture(stage, NULL);
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


void cTexture::CopyFrom(cTexture &src)
{
	if ((m_imageInfo.Height != src.m_imageInfo.Height)
		|| (m_imageInfo.Width != src.m_imageInfo.Width))
		return;

	//D3DLOCKED_RECT slock;
	//if (src.Lock(slock))
	//{
	//	D3DLOCKED_RECT dlock;
	//	if (Lock(dlock))
	//	{
	//		memcpy(dlock.pBits, slock.pBits, slock.Pitch * src.m_imageInfo.Height);
	//		Unlock();
	//	}
	//	src.Unlock();
	//}
}


//void cTexture::CopyFrom(IDirect3DTexture9 *src)
//{
//	RET(!src);
//
//	D3DSURFACE_DESC desc;
//	if (FAILED(src->GetLevelDesc(0, &desc)))
//		return;
//
//	// Check If Texture Match Width - Height
//	if ((m_imageInfo.Height != desc.Height)
//		|| (m_imageInfo.Width != desc.Width))
//		return;
//
//	D3DLOCKED_RECT slock;
//	if (SUCCEEDED(src->LockRect(0, &slock, NULL, 0)))
//	{
//		D3DLOCKED_RECT dlock;
//		if (Lock(dlock))
//		{
//			memcpy(dlock.pBits, slock.pBits, slock.Pitch * m_imageInfo.Height);
//			Unlock();
//		}
//		src->UnlockRect(0);
//	}
//}


//bool cTexture::Lock(D3DLOCKED_RECT &out)
//{
//	//out.pBits = NULL;
//	//RETV(!m_texture, false);
//	//m_texture->LockRect( 0, &out, NULL, 0 );
//	return true;
//}


void cTexture::Unlock()
{
//	m_texture->UnlockRect( 0 );
}


void cTexture::Clear()
{
	m_fileName.clear();
	SAFE_RELEASE(m_texture);
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


// Render Text String on Texture
void cTexture::TextOut(const Str128 &text, const int x, const int y, const DWORD color)
{
	//RET(!m_texture);

	//IDirect3DSurface9* ppSurface = NULL;
	//HDC mDC = NULL;
	//if (m_texture->GetSurfaceLevel(0, &ppSurface) == D3D_OK)
	//{
	//	if (ppSurface->GetDC(&mDC) == D3D_OK)
	//	{
	//		SelectObject(mDC, font.m_font);
	//		SetTextColor(mDC, color);
	//		SetBkMode(mDC, TRANSPARENT);
	//		TextOutA(mDC, x, y, text.c_str(), text.size());
	//		ppSurface->ReleaseDC(mDC);
	//	}
	//	ppSurface->Release();
	//}
}


// Render Text String on Texture
void cTexture::DrawText(const Str128 &text, const sRecti &rect, const DWORD color)
{
	//RET(!m_texture);	

	//IDirect3DSurface9* ppSurface = NULL;
	//HDC mDC = NULL;
	//if (m_texture->GetSurfaceLevel(0, &ppSurface) == D3D_OK)
	//{
	//	if (ppSurface->GetDC(&mDC) == D3D_OK)
	//	{
	//		SelectObject(mDC, font.m_font);
	//		SetTextColor(mDC, color);
	//		SetBkMode(mDC, TRANSPARENT);
	//		::DrawTextA(mDC, text.c_str(), -1, (RECT*)&rect, DT_CENTER | DT_WORDBREAK);
	//		ppSurface->ReleaseDC(mDC);
	//	}
	//	ppSurface->Release();
	//}
}


// Render Text String on Texture using GdiPlus
// https://www.codeproject.com/Articles/42529/Outline-Text
bool cTexture::DrawText2(cRenderer &renderer, const Str128 &szText
	, const cColor &color, const cColor &outlineColor
	, Vector2 &textSize)
{
	//using namespace Gdiplus;
	//Graphics* pGraphics = new Graphics(renderer.m_textMgr.m_graphicBuffer.get());
	//if (!pGraphics)
	//	return false;

	//const int fontSize = 28;
	//pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
	//pGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	//pGraphics->SetPageUnit(UnitPixel);

	//if (!pGraphics)
	//	return false;

	//FontFamily fontFamily(L"Arial");
	//StringFormat strformat;

	//TextDesigner::PngOutlineText text;

	//const Vector4 tColor = color.GetColor();
	//const Vector4 oColor = outlineColor.GetColor();
	//text.TextOutline(Color((int)(tColor.x*255), (int)(tColor.y * 255), (int)(tColor.z * 255))
	//	, Color((int)(oColor.x * 255), (int)(oColor.y * 255), (int)(oColor.z * 255)), 4);
	//text.EnableShadow(false);

	//wstring wstr = str2wstr(szText.c_str());
	//float fWidth = 0.0f;
	//float fHeight = 0.0f;
	//text.MeasureString(pGraphics, &fontFamily, FontStyleBold,
	//	fontSize, wstr.c_str(), Gdiplus::Point(0, 0), &strformat,
	//	NULL, NULL, &fWidth, &fHeight);
	//textSize.x = fWidth;
	//textSize.y = fHeight;

	//std::shared_ptr<Gdiplus::Bitmap> pbmp = renderer.m_textMgr.m_textBuffer;
	//Graphics g(pbmp.get());
	//g.Clear(Gdiplus::Color(0,255,255,255));

	//text.SetPngImage(pbmp);
	//text.DrawString(pGraphics, &fontFamily, FontStyleBold,
	//	fontSize, wstr.c_str(), Gdiplus::Point(0, 0), &strformat);

	//if (pGraphics)
	//{
	//	delete pGraphics;
	//	pGraphics = NULL;
	//}

	//Rect rect1(0, 0, pbmp->GetWidth(), pbmp->GetHeight());
	//BitmapData bitmapData;
	//memset(&bitmapData, 0, sizeof(bitmapData));
	//pbmp->LockBits(
	//	&rect1,
	//	ImageLockModeRead,
	//	PixelFormat32bppARGB,
	//	&bitmapData);

	//int nStride1 = bitmapData.Stride;
	//if (nStride1 < 0)
	//	nStride1 = -nStride1;
	//UINT* pixels = (UINT*)bitmapData.Scan0;
	//if (!pixels)
	//	return false;

	//D3DLOCKED_RECT dlock;
	//if (Lock(dlock))
	//{
	//	UINT* dpixels = (UINT*)dlock.pBits;

	//	for (UINT row = 0; row < bitmapData.Height; ++row)
	//	{
	//		for (UINT col = 0; col < bitmapData.Width; ++col)
	//		{
	//			dpixels[row*bitmapData.Width + col] = pixels[row * nStride1 / 4 + col];
	//		}
	//	}

	//	//memcpy(dlock.pBits, slock.pBits, slock.Pitch * m_imageInfo.Height);
	//	Unlock();
	//}

	//pbmp->UnlockBits(&bitmapData);

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
	RET(!m_customTexture);

	SAFE_RELEASE(m_texture);

	if (m_fileName.empty())
	{
		//Create(renderer, m_imageInfo.Width, m_imageInfo.Height, m_imageInfo.Format);
	}
	else
	{
		Create(renderer, StrPath(m_fileName));
	}
}


bool cTexture::IsLoaded()
{
	return m_texture? true : false;
}
