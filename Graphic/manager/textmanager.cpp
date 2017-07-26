
#include "stdafx.h"
#include "textmanager.h"
#include <mmsystem.h>

using namespace graphic;


cTextManager::cTextManager()
	: m_maxTextCount(128)
	, m_textureSizeX(TEXTURE_SIZEX)
	, m_textureSizeY(TEXTURE_SIZEY)
{
	m_renders.reserve(m_maxTextCount);
	m_buffer.reserve(m_maxTextCount);
	m_cmds.reserve(m_maxTextCount);
}

cTextManager::~cTextManager()
{
	Clear();
}


void cTextManager::Create(const u_int maxTextCount //= 100
	, const int textureSizeX //= 256
	, const int textureSizeY //= 32
)
{
	m_maxTextCount = maxTextCount;
	m_renders.reserve(maxTextCount);
	m_buffer.reserve(maxTextCount);
	m_cmds.reserve(maxTextCount);

	m_textureSizeX = textureSizeX;
	m_textureSizeY = textureSizeY;

	m_graphicBuffer = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(textureSizeX, textureSizeY, PixelFormat32bppARGB));
	m_textBuffer = std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(textureSizeX, textureSizeY, PixelFormat32bppARGB));
}


void cTextManager::NewFrame()
{
	m_renderMap.clear();

	m_renders.clear();

	for (auto &p : m_buffer)
		p->used = false;

	m_cmds.clear();
}


void cTextManager::AddTextRender(cRenderer &renderer, const int id, const Str128 &str
	, const cColor &color //= cColor::WHITE
	, const cColor &outlineColor //= cColor::BLACK
	, BILLBOARD_TYPE::TYPE type //= BILLBOARD_TYPE::Y_AXIS
	, const Transform &tm //= Transform::Identity
	, const int width //=8
	, const int height//=1
)
{
	sText *text = GetCacheText(id);
	if (text)
	{
		text->text.SetTextRect2(renderer, tm, str, color, outlineColor, type);
		text->space = renderer.GetCurrentAlphaBlendSpace();
		text->used = true;

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
		strcpy_s(cmd.str, str.c_str());
		cmd.str[ min(sCommand::MAX_STR - 1, (int)str.size())] = NULL;
		cmd.color = color;
		cmd.outlineColor = outlineColor;
		cmd.type = type;
		cmd.tm = tm;
		cmd.width = width;
		cmd.height = height;
		cmd.space = renderer.GetCurrentAlphaBlendSpace();
		m_cmds.push_back(cmd);
	}
}


// Process Command
void cTextManager::ProcessTextCmd(cRenderer &renderer)
{
	u_int bufferStartIdx = 0;
	for (auto &cmd : m_cmds)
	{
		bool isFindEmptyText = false;

		sText *text = GetCacheText(cmd.id);
		if (text)
		{
			SetCommand2Text(renderer, text, cmd);

			isFindEmptyText = true;
		}
		else
		{
			for (u_int i = bufferStartIdx; i < m_buffer.size(); ++i)
			{
				sText *text = m_buffer[i];
				if ((text->id >= 0) && (text->used))
					continue;

				m_cacheMap.erase(text->id);
				SetCommand2Text(renderer, text, cmd);

				m_renders.push_back(text);

				isFindEmptyText = true;
				bufferStartIdx = i + 1; // set search buffer index
				break;
			}
		}

		if (!isFindEmptyText) // not found empty buffer, create text
		{
			if (m_maxTextCount <= m_buffer.size())
				break; // buffer full, Finish

			cFontGdi *font = cFontManager::Get()->GetFontGdi("test");
			if (!font)
			{
				font = new cFontGdi();
				font->Create();
				cFontManager::Get()->AddFontGdi("test", font);
			}

			sText *text = new sText;
			text->text.Create(renderer, font, cmd.type, cmd.width, cmd.height, m_textureSizeX, m_textureSizeY);
			m_buffer.push_back(text);

			SetCommand2Text(renderer, text, cmd);

			m_renders.push_back(text);
			bufferStartIdx = m_buffer.size();
		}
	}
	m_cmds.clear();

	// clear cache if not use buffer
	set<int> rmIds;
	for (auto kv : m_cacheMap)
	{
		sText *text = kv.second;
		if (text->used)
			continue;
		if (text->id >= 0)
		{
			rmIds.insert(text->id);
			text->id = -1;
		}
	}
	for (auto id : rmIds)
		m_cacheMap.erase(id);

	GarbageCollection();
}


void cTextManager::Render(cRenderer &renderer
	, const bool isSort //= false
)
{
	ProcessTextCmd(renderer);

	if (isSort)
		Sorting();

	// render
	//renderer.SetCullMode(D3DCULL_NONE);
	for (auto &p : m_renders)
		renderer.AddRenderAlpha(p->space, &p->text, p->text.m_quad.m_normal, p->text.m_transform.GetMatrix());
		//p->text.Render(renderer);
	//renderer.SetCullMode(D3DCULL_CCW);
}


void cTextManager::SetCommand2Text(cRenderer &renderer, sText *text, const sCommand &cmd)
{
	text->text.SetTextRect2(renderer, cmd.tm, cmd.str, cmd.color, cmd.outlineColor, cmd.type);

	text->id = cmd.id;
	text->used = true;
	text->space = cmd.space;
	m_cacheMap[cmd.id] = text;
}


cTextManager::sText* cTextManager::GetCacheText(const int id)
{
	auto it = m_cacheMap.find(id);
	if (it == m_cacheMap.end())
		return NULL;
	return it->second;
}


void cTextManager::GarbageCollection()
{
	static int oldT = timeGetTime();
	if (timeGetTime() - oldT < 1000) // check 1 seconds
		return;	
	oldT = timeGetTime();

	set<int> checkIds;
	for (auto &p : m_buffer)
		checkIds.insert(p->id);

	// Error Found
	if (checkIds.size() != m_buffer.size())
	{
		map<int, std::pair<int,sText*>> duplicateCheck;
		for (auto &p : m_buffer)
		{
			++duplicateCheck[p->id].first;
			duplicateCheck[p->id].second = p;
		}

		for (auto &it : duplicateCheck)
		{
			if ((it.first >=0) && (it.second.first > 1))
			{
				it.second.second->id = -1;
				it.second.second->used = false;
				m_cacheMap.erase(it.first);
			}
		}
	}
}


// Text Sorting With Camera Position
// Descent Distance from Camera
void cTextManager::Sorting()
{
	cCamera *cam = GetMainCamera();
	Vector3 camOrig, camDir;
	cam->GetRay(camOrig, camDir);

	std::sort(m_renders.begin(), m_renders.end(), 
		[&](const sText* a, const sText* b)
		{
			const float l1 = a->text.m_transform.pos.LengthRoughly(camOrig);
			const float l2 = b->text.m_transform.pos.LengthRoughly(camOrig);
			return l1 > l2;
		}
	);
}


void cTextManager::Clear()
{
	for (auto &p : m_buffer)
		delete p;
	m_buffer.clear();

	m_graphicBuffer = NULL;
	m_textBuffer = NULL;
}
