
#include "../stdafx.h"
#include "dockmanager.h"
#include "renderwindow.h"
#include "dockwindow.h"


using namespace graphic;
using namespace framework;

cDockManager::cDockManager()
	: m_dockTarget(NULL)
	, m_mainWindow(NULL)
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


cRenderWindow* cDockManager::NewRenderWindow(const string &title, const int width, const int height
	, cRenderer *shared)
{
	cRenderWindow *window = NULL;
	if (m_poolWindow.empty())
	{
		window = new cRenderWindow();
		window->Create(title, width, height, shared);
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


// deAllocation cDockWindow Object
void cDockManager::DeleteDockWindow(cDockWindow *p)
{
	p->Clear();
	m_poolDock.push_back(p);
}


void cDockManager::UpdateRender(const float deltaSeconds)
{
	UpdateModified();

	m_dockTarget = NULL; // this must be here!!
	m_dragWindow = NULL;

	for (auto &p : m_windows)
	{
		p->TranslateEvent();
		p->Update(deltaSeconds);
		p->Render(deltaSeconds);
	}

	if (m_dockTarget && m_dragWindow)
	{
		cDockWindow *src = m_dragWindow->m_dock;
		src->Undock(false);
		m_dockTarget->Dock(m_dockSlot, src);
	}

	//for (auto &p : m_windows)
	//	p->DragAndDrop(deltaSeconds);
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

	for (auto &p : m_docks)
		delete p;
	m_docks.clear();

	for (auto &p : m_poolDock)
		delete p;
	m_poolDock.clear();
}


void cDockManager::SetDragState(cRenderWindow *drag
	, const bool isDragStart) // = true
{
	if (isDragStart)
	{
		dbg::Print("start drag\n");
		m_dockTarget = NULL;
	}
	else
	{
		m_dragWindow = drag;
		//if (m_dockTarget)
		//{
		//	cDockWindow *src = drag->m_dock;
		//	src->Undock(false);
		//	m_dockTarget->Dock(m_dockSlot, src);
		//}
	}

	for (auto &p : m_windows)
	{
		if (!isDragStart)
			p->SetFinishDragBindState();
		else if (drag != p)
			p->SetDragBindState();
	}
}


void cDockManager::SetDragTarget(cDockWindow *dock, eDockType::Enum dockSlot)
{
	m_dockTarget = dock;
	m_dockSlot = dockSlot;
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

	for (auto &p : m_docks)
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

	for (auto &p : m_docks)
		p->ResetDevice(shared);

	for (auto &p : m_poolDock)
		p->ResetDevice(shared);
}
