
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


cFontGdi* cFontManager::GetFontGdi(const Str64 &name)
{
	auto it = m_fontGdis.find(name.GetHashCode());
	if (m_fontGdis.end() == it)
		return NULL;
	return it->second;
}


bool cFontManager::AddFontGdi(const Str64 &name, cFontGdi *font)
{
	auto it = m_fontGdis.find(name.GetHashCode());
	if (m_fontGdis.end() != it)
		return false;

	m_fontGdis[name.GetHashCode()] = font;
	return true;
}


void cFontManager::Clear()
{
	for (auto p : m_fontGdis)
		delete p.second;
	m_fontGdis.clear();
}
