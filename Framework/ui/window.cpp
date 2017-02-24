
#include "stdafx.h"
#include "window.h"

using namespace framework;
using namespace graphic;

cWindow::cWindow(cSprite &sprite, const int id, const string &name) : // name="window"
	cSprite2(sprite, id, name)
,	m_isVisible(true)
{
}

cWindow::~cWindow()
{
}


bool cWindow::Init(cRenderer &renderer)
{
	// 파생클래스에서 구현한다.
	return true;
}


bool cWindow::Update(const float deltaSeconds)
{
	return cSprite2::Update(deltaSeconds);
}


void cWindow::Render(cRenderer &renderer, const Matrix44 &parentTm)
{
	cSprite2::Render(renderer, parentTm);
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
