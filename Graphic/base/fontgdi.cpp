
#include "stdafx.h"
#include "fontgdi.h"

using namespace graphic;

cFontGdi::cFontGdi()
	: m_font(NULL)
{
}

cFontGdi::~cFontGdi()
{
	if (m_font)
		DeleteObject(m_font);
}


bool cFontGdi::Create(const int height, const int width)
{
	m_font = CreateFontA(height, width, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");

	return true;
}
