
#include "stdafx.h"
#include "window.h"

using namespace framework;


cWindow::cWindow(LPD3DXSPRITE sprite, const int id, const string &name) : // name="window"
	graphic::cSprite(sprite, id, name)
,	m_isVisible(true)
{
}

cWindow::~cWindow()
{
}


bool cWindow::Init(graphic::cRenderer &renderer)
{
	// 파생클래스에서 구현한다.
	return true;
}


bool cWindow::Update(const float deltaSeconds)
{
	return graphic::cSprite::Update(deltaSeconds);
}


void cWindow::Render(graphic::cRenderer &renderer, const Matrix44 &parentTm)
{
	graphic::cSprite::Render(renderer, parentTm);
}


// 메세지 이벤트 처리.
bool cWindow::MessageProc( UINT message, WPARAM wParam, LPARAM lParam)
{
	// child window message process
	BOOST_REVERSE_FOREACH (auto &node, m_children)
	{
		if (cWindow *childWnd = dynamic_cast<cWindow*>(node))
		{
			if (childWnd->MessageProc(message, wParam, lParam))
				break;
		}
	}
	
	return false;
}


// 이벤트 핸들러 호출
bool cWindow::DispatchEvent( cEvent &event )
{
	cEventHandler::SearchEventTable(event);
	if (!event.IsSkip() && m_parent) // 이벤트가 스킵되지 않았다면 부모로 올라가서 실행한다.
	{
		if (cWindow *parent = dynamic_cast<cWindow*>(m_parent))
			parent->DispatchEvent(event);
	}
	return true;
}
