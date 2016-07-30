
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
	// �Ļ�Ŭ�������� �����Ѵ�.
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


// �޼��� �̺�Ʈ ó��.
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


// �̺�Ʈ �ڵ鷯 ȣ��
bool cWindow::DispatchEvent( cEvent &event )
{
	cEventHandler::SearchEventTable(event);
	if (!event.IsSkip() && m_parent) // �̺�Ʈ�� ��ŵ���� �ʾҴٸ� �θ�� �ö󰡼� �����Ѵ�.
	{
		if (cWindow *parent = dynamic_cast<cWindow*>(m_parent))
			parent->DispatchEvent(event);
	}
	return true;
}