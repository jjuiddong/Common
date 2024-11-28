
#include "stdafx.h"
#include "fontmanager.h"

using namespace graphic;


cFontManager::cFontManager()
{
}

cFontManager::~cFontManager()
{
	Clear();
}


std::pair<IFW1FontWrapper*, IDWriteTextFormat*> cFontManager::GetFont(cRenderer &renderer
	, const char *name //= "Arial"
	, const float fontSize // = 16.0f
)
{
	IFW1FontWrapper *fontWrapper = NULL;
	WStr64 wname = Str64(name).wstr();

	const hashcode hcode = (hashcode)Str64(name).GetHashCode();
	auto it = m_fonts.find(hcode);
	if (m_fonts.end() == it)
	{
		IFW1Factory *pFW1Factory;
		HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
		if (FAILED(hResult))
			return { NULL,NULL };

		hResult = pFW1Factory->CreateFontWrapper(renderer.GetDevice()
			, wname.c_str(), &fontWrapper);

		if (FAILED(hResult))
			return{ NULL,NULL };

		SAFE_RELEASE(pFW1Factory);

		m_fonts[hcode] = fontWrapper;
	}
	else
	{
		fontWrapper = it->second;
	}

	IDWriteTextFormat *textFormat = NULL;

	auto it2 = m_textFormats.find({ hcode, fontSize });
	if (m_textFormats.end() == it2)
	{
		// Get the DirectWrite factory used by the font-wrapper
		IDWriteFactory *pDWriteFactory;
		HRESULT hResult = fontWrapper->GetDWriteFactory(&pDWriteFactory);

		// Create the default DirectWrite text format to base layouts on
		hResult = pDWriteFactory->CreateTextFormat(
			wname.c_str(),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"",
			&textFormat
		);
		if (FAILED(hResult))
			return { NULL, NULL };

		SAFE_RELEASE(pDWriteFactory);

		m_textFormats[{hcode, fontSize}] = textFormat;
	}
	else
	{
		textFormat = it2->second;
	}

	return{ fontWrapper, textFormat };
}


bool cFontManager::AddFont(const char *name, IFW1FontWrapper *font)
{
	const hashcode hcode = Str64(name).GetHashCode();
	auto it = m_fonts.find(hcode);
	if (m_fonts.end() != it)
		return false;

	m_fonts[hcode] = font;
	return true;
}


bool cFontManager::AddTextFormat(const char *name, const float fontSize, IDWriteTextFormat *textFormat)
{
	const hashcode hcode = Str64(name).GetHashCode();
	auto it = m_textFormats.find({ hcode, fontSize });
	if (m_textFormats.end() != it)
		return false;

	m_textFormats[{ hcode, fontSize }] = textFormat;
	return true;
}


void cFontManager::Clear()
{
	for (auto p : m_fonts)
		SAFE_RELEASE(p.second);
	m_fonts.clear();

	for (auto p : m_textFormats)
		SAFE_RELEASE(p.second);
	m_textFormats.clear();
}

