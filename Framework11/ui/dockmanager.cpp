
#include "stdafx.h"
#include "dockmanager.h"
#include "renderwindow.h"
#include "dockwindow.h"


using namespace graphic;
using namespace framework;

cDockManager::cDockManager()
	: m_dockTarget(NULL)
	, m_mainWindow(NULL)
	, m_state(eState::NORMAL)
{
}

cDockManager::~cDockManager()
{
	Clear();
}


// this function must call First windows creation
void cDockManager::InsertMainRenderWindow(cRenderWindow *main)
{
	m_mainWindow = main;
	m_windows.push_back(main);
}


cRenderWindow* cDockManager::NewRenderWindow(const StrId &title, const int width, const int height
	, cRenderer *shared)
{
	cRenderWindow *window = NULL;
	if (m_poolWindow.empty())
	{
		window = new cRenderWindow();
		window->Create(title, width, height, shared, false);
	}
	else
	{
		window = m_poolWindow.back();
		m_poolWindow.pop_back();
		window->WakeUp(title, width, height);
	}

	m_addWindows.push_back(window);
	return window;
}


void cDockManager::DeleteRenderWindow(cRenderWindow *wnd)
{
	m_rmWindows.push_back(wnd);
}


// Allocation cDockWindow Object
cDockWindow* cDockManager::NewDockWindow()
{
	cDockWindow *p = NULL;
	if (m_poolDock.empty())
	{
		p = new cDockWindow();
	}
	else
	{
		p = m_poolDock.back();
		m_poolDock.pop_back();
	}

	return p;
}


// DeAllocation cDockWindow Object
void cDockManager::DeleteDockWindow(cDockWindow *p)
{
	p->ClearConnection();
	m_poolDock.push_back(p);
}


void cDockManager::UpdateRender(const float deltaSeconds)
{
	UpdateModified();

	for (auto &p : m_windows)
	{
		p->TranslateEvent();
		p->Update(deltaSeconds);
		p->Render(deltaSeconds);
	}

	// Docking Window Process
	if (eState::DRAG_END == m_state)
	{
		if (m_dockTarget && m_dragWindow && (m_dockTarget->m_dragSlot != eDockSlot::NONE))
		{
			cDockWindow *src = m_dragWindow->m_dock;
			src->Undock(false);
			m_dockTarget->Dock(m_dockTarget->m_dragSlot, src);
		}

		m_dockTarget = NULL;
		m_dragWindow = NULL;
		m_state = eState::NORMAL;
	}
}

void cDockManager::UpdateModified()
{
	if (!m_addWindows.empty())
	{
		for (auto &p : m_addWindows)
			m_windows.push_back(p);
		m_addWindows.clear();
	}

	if (!m_rmWindows.empty())
	{
		for (auto &p : m_rmWindows)
		{
			common::removevector(m_windows, p);
			p->Sleep();
			m_poolWindow.push_back(p);
		}
		m_rmWindows.clear();
	}
}


void cDockManager::Clear()
{
	UpdateModified();

	for (auto &p : m_windows)
		delete p;
	m_windows.clear();

	for (auto &p : m_poolWindow)
		delete p;
	m_poolWindow.clear();

	for (auto &p : m_poolDock)
		delete p;
	m_poolDock.clear();
}


void cDockManager::SetMoveState(cRenderWindow *drag
	, const bool isMove //= true
)
{
	m_state = isMove ? eState::MOVE : eState::NORMAL;
}


void cDockManager::SetDragState(cRenderWindow *drag
	, const bool isDragStart // = true
)
{
	if (isDragStart)
	{
		dbg::Print("start drag\n");
		m_dockTarget = NULL;
		m_state = eState::DRAG;
	}
	else
	{
		m_dragWindow = drag;
		m_state = eState::DRAG_END;
	}

	for (auto &p : m_windows)
	{
		if (!isDragStart && (drag != p))
			p->SetFinishDragBindState();
		else if (drag != p)
			p->SetDragBindState();
	}
}


void cDockManager::SetDragTarget(cDockWindow *dock)
{
	m_dockTarget = dock;
}


bool cDockManager::IsDragState()
{
	return m_state == eState::DRAG;
}


bool cDockManager::IsMoveState()
{
	return m_state == eState::MOVE;
}


void cDockManager::LostDevice()
{
	for (auto &p : m_windows)
	{
		if (m_mainWindow != p)
			p->LostDevice();
	}

	for (auto &p : m_poolWindow)
		p->LostDevice();

	for (auto &p : m_poolDock)
		p->LostDevice();
}


void cDockManager::ResetDevice(cRenderer *shared) // =NULL
{
	for (auto &p : m_windows)
	{
		if (m_mainWindow != p)
			p->ResetDevice(shared);
	}

	for (auto &p : m_poolWindow)
		p->ResetDevice(shared);

	for (auto &p : m_poolDock)
		p->ResetDevice(shared);
}


cRenderWindow* cDockManager::GetMainWindow()
{
	return m_mainWindow;
}


graphic::cRenderer* cDockManager::GetMainRenderer()
{
	RETV(!m_mainWindow, NULL);
	return &m_mainWindow->m_renderer;
}
