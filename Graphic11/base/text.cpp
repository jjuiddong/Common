
#include "stdafx.h"
#include "text.h"

using namespace graphic;


cText::cText()
	: m_color(cColor::WHITE.GetAbgr())
	, m_fontWrapper(NULL)
	, m_textLayout(NULL)
{
}

cText::cText(cRenderer &renderer
	, const float fontSize //= fontSize=18
	, const bool isBold //= isBold=true
	, const char *fontName //=±¼¸²
	, const cColor &color //=cColor::WHITE
	, const wchar_t *text //= NULL
)
	: m_fontWrapper(NULL)
	, m_textLayout(NULL)
	, m_color(color.GetAbgr())
{
	Create(renderer, fontSize, isBold, fontName, color, text);
}


cText::~cText()
{
	Clear();
}


// ÅØ½ºÆ® »ý¼º.
bool cText::Create(cRenderer &renderer
	, const float fontSize //=18
	, const bool isBold //=true
	, const char *fontName //=±¼¸²
	, const cColor &color //=cColor::WHITE
	, const wchar_t *text //=NULL
)
{
	Clear();

	auto result = renderer.m_fontMgr.GetFont(renderer, fontName, fontSize);
	IFW1FontWrapper *fontWrapper = result.first;
	IDWriteTextFormat *textFormat = result.second;
	RETV(!fontWrapper, false);

	if (text)
	{
		IDWriteFactory *pDWriteFactory;
		HRESULT hResult = fontWrapper->GetDWriteFactory(&pDWriteFactory);

		pDWriteFactory->CreateTextLayout(text,wcslen(text),textFormat,0.0f,0.0f,&m_textLayout);

		// No word wrapping
		m_textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		m_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		pDWriteFactory->Release();
	}

	m_fontWrapper = fontWrapper;
	m_fontSize = fontSize;
	m_color = color.GetAbgr();

	if (text)
		m_text = text;

	return true;
}


void cText::Render(cRenderer &renderer
	, const float x, const float y, const wchar_t *text
	, const bool restore //= false
)
{
	RET(!m_fontWrapper);

	m_fontWrapper->DrawString(
		renderer.GetDevContext()
		, text
		, m_fontSize
		, x, y
		, m_color
		, (restore)? (FW1_CENTER | FW1_VCENTER | FW1_RESTORESTATE) : (FW1_CENTER | FW1_VCENTER)
	);

}


void cText::Render(cRenderer &renderer
	, const float x, const float y
	, const bool restore //= false
)
{
	RET(!m_fontWrapper);

	if (m_textLayout)
	{
		m_fontWrapper->DrawTextLayout(renderer.GetDevContext(), m_textLayout, x, y, m_color, 0);
	}
	else
	{
		m_fontWrapper->DrawString(
			renderer.GetDevContext()
			, m_text.c_str()
			, m_fontSize
			, x, y
			, m_color
			, (restore) ? (FW1_CENTER | FW1_VCENTER | FW1_RESTORESTATE) : (FW1_CENTER | FW1_VCENTER)
		);
	}
}


void cText::Clear()
{
	m_text.clear();
	SAFE_RELEASE(m_textLayout);
}
