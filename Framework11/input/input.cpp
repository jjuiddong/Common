
#include "stdafx.h"
#include "input.h"

using namespace framework;


cInputManager::cInputManager()
	: m_lBtnDown(false)
	, m_rBtnDown(false)
	, m_mBtnDown(false)
	, m_lBtnDbClick(false)
	, m_clickTime(0)
	, m_dbClickTime(0)
{
	m_mousePt.x = 0;
	m_mousePt.y = 0;
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
			m_lBtnDown = true;
			m_clickTime = curT;
		}
	}
	break;

	case WM_LBUTTONUP:
	{
		const int curT = GetTickCount();
		if (curT - m_dbClickTime > 200)
		{
			m_lBtnDown = false;
		}
	}
	break;

	case WM_RBUTTONDOWN: m_rBtnDown = true; break;
	case WM_RBUTTONUP: m_rBtnDown = false; break;
	case WM_MBUTTONDOWN: m_mBtnDown = true; break;
	case WM_MBUTTONUP: m_mBtnDown = false; break;
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
				m_lBtnDown = true;
				m_clickTime = curT;
			}
		}
		break;
		case sf::Mouse::Right:
			m_rBtnDown = true;
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
				m_lBtnDown = false;
			}
		}
		break;
		case sf::Mouse::Right:
			m_rBtnDown = false;
			break;
		}
		break;

	case sf::Event::MouseMoved:
		break;
	}
}


bool cInputManager::IsClick()
{
	return m_lBtnDown;
}
