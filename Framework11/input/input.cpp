
#include "stdafx.h"
#include "input.h"

using namespace framework;


cInputManager::cInputManager()
	: m_lBtnDbClick(false)
	, m_clickTime(0)
	, m_dbClickTime(0)
{
	m_mousePt.x = 0;
	m_mousePt.y = 0;
	ZeroMemory(m_mouseDown, sizeof(m_mouseDown));
	ZeroMemory(m_mouseClicked, sizeof(m_mouseClicked));	
}

cInputManager::~cInputManager()
{
}


// 매 프래임마다 먼저 호출해야 한다.
void cInputManager::NewFrame()
{
	m_mouseClicked[0] = false;
	m_mouseClicked[1] = false;
	m_mouseClicked[2] = false;
}


void cInputManager::Update(const float deltaSeconds)
{
	m_lBtnDbClick = false;
}


void cInputManager::MouseProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		const int curT = GetTickCount();
		if ((curT - m_clickTime) < 300) // Double Click Check
		{
			m_lBtnDbClick = true;
			m_dbClickTime = curT;
		}

		if (curT - m_dbClickTime > 200)
		{
			m_mouseDown[0] = true;
			m_clickTime = curT;
		}
	}
	break;

	case WM_LBUTTONUP:
	{
		const int curT = GetTickCount();
		if (curT - m_dbClickTime > 200)
		{
			m_mouseDown[0] = false;
			m_mouseClicked[0] = true;
		}
	}
	break;

	case WM_RBUTTONDOWN: m_mouseDown[1] = true; break;
	case WM_RBUTTONUP: m_mouseDown[1] = false; break;
	case WM_MBUTTONDOWN: m_mouseDown[2] = true; break;
	case WM_MBUTTONUP: m_mouseDown[2] = false; break;
	}
}


void cInputManager::MouseProc(const sf::Event &evt)
{
	m_lBtnDbClick = false;

	switch (evt.type)
	{
	case sf::Event::MouseButtonPressed:
		switch (evt.mouseButton.button)
		{
		case sf::Mouse::Left:
		{
			const POINT pt = { evt.mouseButton.x, evt.mouseButton.y };
			const float len = (float)sqrt((pt.x - m_mousePt.x) * (pt.x - m_mousePt.x) + (pt.y - m_mousePt.y) * (pt.y - m_mousePt.y));
			const bool isNear = len < 10;

			const int curT = GetTickCount();
			if (isNear && ((curT - m_clickTime) < 300)) // Double Click Check
			{
				m_lBtnDbClick = true;
				m_dbClickTime = curT;
			}

			if (curT - m_dbClickTime > 200)
			{
				m_mouseDown[0] = true;
				m_clickTime = curT;
			}
		}
		break;
		case sf::Mouse::Right:
			m_mouseDown[1] = true;
			break;
		}
		m_mousePt = { evt.mouseButton.x, evt.mouseButton.y };
		m_mouseClickPt = { evt.mouseButton.x, evt.mouseButton.y };
		break;

	case sf::Event::MouseButtonReleased:
		switch (evt.mouseButton.button)
		{
		case sf::Mouse::Left:
		{
			const POINT pt = { evt.mouseButton.x, evt.mouseButton.y };
			const float len = (float)sqrt((pt.x - m_mouseClickPt.x) * (pt.x - m_mouseClickPt.x) + (pt.y - m_mouseClickPt.y) * (pt.y - m_mouseClickPt.y));
			const bool isNear = len < 10;

			const int curT = GetTickCount();
			if (curT - m_dbClickTime > 200)
			{
				m_mouseDown[0] = false;

				if (isNear && ((curT - m_clickTime) < 300))
					m_mouseClicked[0] = true;
			}
		}
		break;
		case sf::Mouse::Right:
			m_mouseDown[1] = false;
			break;
		}
		m_mousePt = { evt.mouseButton.x, evt.mouseButton.y };
		break;

	case sf::Event::MouseMoved:
		m_mousePt = { evt.mouseMove.x, evt.mouseMove.y };
		break;
	}
}


bool cInputManager::IsClick()
{
	return m_mouseClicked[0];
}


POINT cInputManager::GetDockWindowCursorPos(cDockWindow *dock)
{
	RETV2(!dock, {})

	POINT pos = m_mousePt;
	pos.x -= (int)dock->m_rect.left;
	pos.y -= (int)dock->m_rect.top;
	return pos;
}
