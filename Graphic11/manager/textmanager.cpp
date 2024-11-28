
#include "stdafx.h"
#include "textmanager.h"
#include <mmsystem.h>

using namespace graphic;


cTextManager::cTextManager()
	: m_maxTextCount(128)
	, m_textureSizeX(TEXTURE_SIZEX)
	, m_textureSizeY(TEXTURE_SIZEY)
	, m_generateCount(0)
	, m_cacheCount(0)
	, m_timePrevSecond(0)
	, m_timeLoadBalance(0)
{
	m_renders.reserve(m_maxTextCount);
	m_buffer.reserve(m_maxTextCount);
	m_codes.reserve(m_maxTextCount);
	m_delayGens.reserve(m_maxTextCount);
	m_timer.Create();
}

cTextManager::~cTextManager()
{
	Clear();
}


void cTextManager::Create(const uint maxTextCount //= 100
	, const int textureSizeX //= TEXTURE_SIZEX
	, const int textureSizeY //= TEXTURE_SIZEY
)
{
	m_maxTextCount = maxTextCount;
	m_renders.reserve(maxTextCount);
	m_buffer.reserve(maxTextCount);
	m_codes.reserve(maxTextCount);
	m_delayGens.reserve(maxTextCount);

	m_textureSizeX = textureSizeX;
	m_textureSizeY = textureSizeY;

	m_graphicBmp = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(textureSizeX, textureSizeY, PixelFormat32bppARGB));
	m_textBmp = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(textureSizeX, textureSizeY, PixelFormat32bppARGB));
	m_graphic = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(m_graphicBmp.get()));
	m_graphicText = std::shared_ptr<Gdiplus::Graphics>(new Gdiplus::Graphics(m_textBmp.get()));
}


void cTextManager::NewFrame()
{
	m_generateCount = 0;
	m_cacheCount = 0;

	m_renderMap.clear();

	m_renders.clear();

	for (auto &p : m_buffer)
		p->used = false;

	m_codes.clear();
}


void cTextManager::AddTextRender(cRenderer &renderer
	, const __int64 id
	, const wchar_t *str
	, const cColor &color //= cColor::WHITE
	, const cColor &outlineColor //= cColor::BLACK
	, BILLBOARD_TYPE::TYPE type //= BILLBOARD_TYPE::Y_AXIS
	, const Transform &tm //= Transform::Identity
	, const bool isDepthNone //= false
	, const int width //=16
	, const int height//=1
	, const float dynScaleMin //= 0.5f
	, const float dynScaleMax //= 200.5f
	, const float dynScaleAlpha //= 1.f
)
{
	const cColor c1 = color.GetAbgr();
	const cColor c2 = outlineColor.GetAbgr();

	sText *text = GetCacheText(id);
	if (text)
	{
		// cache에 있는 것은, 대부분 고정된 문자열이기 때문에, 다시 업데이트 될 일이
		// 거의 없으므로, 바뀌는 문자열을 바로 업데이트 해도 문제가 없다.
		//if (text->gen)
		{
			if (text->text.SetTextRect(renderer, tm, str, c1, c2, type))
				++m_generateCount;
			else
				++m_cacheCount;
		}

		text->space = renderer.GetCurrentAlphaBlendSpace();
		text->used = true;
		//text->gen = true;
		text->depthNone = isDepthNone;
		text->initTime = m_timer.GetSeconds();
		text->text.m_quad.m_dynScaleMin = dynScaleMin;
		text->text.m_quad.m_dynScaleMax = dynScaleMax;
		text->text.m_quad.m_dynScaleAlpha = dynScaleAlpha;

		if (m_renderMap.end() == m_renderMap.find(id))
		{
			m_renderMap[id] = text;
			m_renders.push_back(text);
		}
	}
	else
	{
		sCommand cmd;
		cmd.id = id;
		cmd.str = str;
		cmd.color = c1;
		cmd.outlineColor = c2;
		cmd.type = type;
		cmd.tm = tm;
		cmd.depthNone = isDepthNone;
		cmd.width = width;
		cmd.height = height;
		cmd.dynScaleMin = dynScaleMin;
		cmd.dynScaleMax = dynScaleMax;
		cmd.dynScaleAlpha = dynScaleMax;
		cmd.space = renderer.GetCurrentAlphaBlendSpace();
		m_codes.push_back(cmd);
	}
}


// Process Command
void cTextManager::ProcessTextCmd(cRenderer &renderer)
{
	size_t bufferStartIdx = 0;
	for (auto &cmd : m_codes)
	{
		bool isFindEmptyText = false;

		sText *text = GetCacheText(cmd.id);
		if (text)
		{
			SetCommand2Text(renderer, text, cmd);

			isFindEmptyText = true;
			++m_cacheCount;
		}
		else
		{
			for (size_t i = bufferStartIdx; i < m_buffer.size(); ++i)
			{
				sText *text = m_buffer[i];
				if ((text->id >= 0) && (text->used))
					continue;

				if (m_cacheMap.end() != m_cacheMap.find(text->id))
					continue; // if exist cache, next text

				SetCommand2Text(renderer, text, cmd);

				//m_renders.push_back(text);

				isFindEmptyText = true;
				bufferStartIdx = i + 1; // set search buffer index
				break;
			}
		}

		if (!isFindEmptyText) // not found empty buffer, create text
		{
			if (m_maxTextCount <= m_buffer.size())
				break; // buffer full, Finish

			sText *text = new sText;
			text->text.Create(renderer, cmd.type, cmd.width, cmd.height
				, m_textureSizeX, m_textureSizeY, cmd.dynScaleMin, cmd.dynScaleMax
				, cmd.dynScaleAlpha);
			m_buffer.push_back(text);

			SetCommand2Text(renderer, text, cmd);

			//m_renders.push_back(text);
			bufferStartIdx = m_buffer.size();
		}
	}
	m_codes.clear();

	// clear cache if not use buffer
	const double curT = m_timer.GetSeconds();
	set<__int64> rmIds;
	for (auto kv : m_cacheMap)
	{
		sText *text = kv.second;
		if (text->used)
			continue;
		if (text->id >= 0)
		{
			if ((curT - text->initTime) < 3.f)
				continue;
			rmIds.insert(text->id);
			text->id = -1;
			//text->gen = false;
		}
	}
	for (auto id : rmIds)
		m_cacheMap.erase(id);

	GarbageCollection();
}


void cTextManager::DelayGenerateText(cRenderer &renderer)
{
	const double curT = m_timer.GetSeconds();
	if (curT - m_timePrevSecond > 1.f)
	{
		m_timePrevSecond = curT;
		m_timeLoadBalance = 0;
	}

	while (!m_delayGens.empty() && (m_timeLoadBalance < 0.033f))
	{
		sDelayGenerateText text = m_delayGens.back();
		m_delayGens.pop_back();
		m_delayGenSet.erase(text.id);

		if (!text.stext->used)
			continue;

		const double t0 = m_timer.GetSeconds();
		{
			if (text.stext->text.SetTextRect(renderer, text.tm, text.str.c_str()
				, text.color, text.outlineColor, text.type))
			{
				++m_generateCount;
			}
			else
			{
				++m_cacheCount;
			}
		}
		const double t1 = m_timer.GetSeconds();

		m_timeLoadBalance += (t1 - t0);

		//text.stext->gen = true;
	}

	assert(m_delayGens.size() == m_delayGenSet.size());
}


void cTextManager::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const bool isSort //= false
)
{
	ProcessTextCmd(renderer);
	DelayGenerateText(renderer);

	if (isSort)
		Sorting();

	const Matrix44 tm = parentTm;
	sAlphaBlendSpace *alphaSpace = NULL;
	for (auto &p : m_renders)
	{
		p->text.m_alphaNormal = p->text.m_quad.m_normal;
		p->text.m_isDepthNone = p->depthNone;
		p->text.SetRenderFlag(eRenderFlag::NODEPTH, p->depthNone);

		if (p->space != alphaSpace)
		{
			// 중복 업데이트를 막기위해 처리함.
			alphaSpace = p->space;
			alphaSpace->parentTm = tm;
		}
		renderer.AddRenderAlpha(p->space, &p->text);// , tm);// p->text.m_transform.GetMatrix());
	}
}


void cTextManager::SetCommand2Text(cRenderer &renderer, sText *text, const sCommand &cmd)
{
	if (m_delayGenSet.end() == m_delayGenSet.find(cmd.id))
	{
		sDelayGenerateText delayTxt;
		delayTxt.id = cmd.id;
		delayTxt.str = cmd.str;
		delayTxt.type = cmd.type;
		delayTxt.color = cmd.color;
		delayTxt.outlineColor = cmd.outlineColor;
		delayTxt.tm = cmd.tm;
		delayTxt.stext = text;
		m_delayGens.push_back(delayTxt);
		m_delayGenSet.insert(cmd.id);
	}

	//if (text->text.SetTextRect(renderer, cmd.tm, cmd.str.c_str(), cmd.color, cmd.outlineColor, cmd.type))
	//	++m_generateCount;
	//else
	//	++m_cacheCount;

	text->id = cmd.id;
	text->used = true;
	//text->gen = true;
	text->depthNone = cmd.depthNone;
	text->space = cmd.space;
	text->initTime = m_timer.GetSeconds();
	text->text.m_quad.m_dynScaleMin = cmd.dynScaleMin;
	text->text.m_quad.m_dynScaleMax = cmd.dynScaleMax;
	text->text.m_quad.m_dynScaleAlpha = cmd.dynScaleAlpha;
	m_cacheMap[cmd.id] = text;
}


cTextManager::sText* cTextManager::GetCacheText(const __int64 id)
{
	auto it = m_cacheMap.find(id);
	if (it == m_cacheMap.end())
		return NULL;
	return it->second;
}


void cTextManager::GarbageCollection()
{
	static double oldT = m_timer.GetSeconds();
	const double curT = m_timer.GetSeconds();
	if (curT - oldT < 1.f) // check 1 seconds
		return;	
	oldT = curT;

	set<__int64> checkIds;
	for (auto &p : m_buffer)
		checkIds.insert(p->id);

	// Error Found
	if (checkIds.size() != m_buffer.size())
	{
		map<__int64, std::pair<int,sText*>> duplicateCheck;
		for (auto &p : m_buffer)
		{
			++duplicateCheck[p->id].first;
			duplicateCheck[p->id].second = p;
		}

		for (auto &it : duplicateCheck)
		{
			if ((it.first >= 0) && (it.second.first > 1))
			{
				sText* stext = it.second.second;
				stext->id = -1;
				stext->used = false;
				m_cacheMap.erase(it.first);
			}
		}
	}
}


// Text Sorting With Camera Position
// Descent Distance from Camera
void cTextManager::Sorting()
{
	cCamera &cam = GetMainCamera();
	const Ray ray = cam.GetRay();

	std::sort(m_renders.begin(), m_renders.end(), 
		[&](const sText* a, const sText* b)
		{
			const float l1 = a->text.m_transform.pos.LengthRoughly(ray.orig);
			const float l2 = b->text.m_transform.pos.LengthRoughly(ray.orig);
			return l1 > l2;
		}
	);
}


// 캐시를 초기화 한다.
// 문자열을 다시 그리기 위해서 사용된다.
void cTextManager::ClearCache()
{
	m_cacheMap.clear();
	for (auto &text : m_buffer)
	{
		text->text.m_text.clear();
		text->text.m_color = cColor::BLACK;
	}
}


void cTextManager::Clear()
{
	for (auto &p : m_buffer)
		delete p;
	m_buffer.clear();

	m_graphicBmp = NULL;
	m_textBmp = NULL;
	m_graphic = NULL;
	m_graphicText = NULL;
}
