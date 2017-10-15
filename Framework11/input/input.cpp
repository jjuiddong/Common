
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
}

cInputManager::~cInputManager()
{
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
	switch (evt.type)
	{
	case sf::Event::MouseButtonPressed:
		switch (evt.mouseButton.button)
		{
		case sf::Mouse::Left:
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
		case sf::Mouse::Right:
			m_mouseDown[1] = true;
			break;
		}
		break;

	case sf::Event::MouseButtonReleased:
		switch (evt.mouseButton.button)
		{
		case sf::Mouse::Left:
		{
			const int curT = GetTickCount();
			if (curT - m_dbClickTime > 200)
			{
				m_mouseDown[0] = false;
			}
		}
		break;
		case sf::Mouse::Right:
			m_mouseDown[1] = false;
			break;
		}
		break;

	case sf::Event::MouseMoved:
		m_mousePt = { evt.mouseMove.x, evt.mouseMove.y };
		break;
	}
}


bool cInputManager::IsClick()
{
	return m_mouseDown[0];
}


POINT cInputManager::GetDockWindowCursorPos(cDockWindow *dock)
{
	RETV2(!dock, {})

	POINT pos = m_mousePt;
	pos.x -= (int)dock->m_rect.left;
	pos.y -= (int)dock->m_rect.top;
	return pos;
}
