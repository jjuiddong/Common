//
// 2017-04-21, jjuiddong
// docking system manager
//
#pragma once

#include "dock.h"


namespace framework
{
	class cRenderWindow;
	class cDockWindow;

	class cDockManager : public common::cSingleton<cDockManager>
	{
	public:
		cDockManager();
		virtual ~cDockManager();

		void InsertMainRenderWindow(cRenderWindow *main);
		cRenderWindow* NewRenderWindow(const string &title, const int width, const int height
			, graphic::cRenderer *shared);
		void DeleteRenderWindow(cRenderWindow *wnd);
		cDockWindow* NewDockWindow();
		void DeleteDockWindow(cDockWindow *p);
		void UpdateRender(const float deltaSeconds);
		void Clear();
		void SetDragState(cRenderWindow *drag, const bool isDragStart=true);
		void SetDragTarget(cDockWindow *target, eDockType::Enum dockSlot);
		void LostDevice();
		void ResetDevice(graphic::cRenderer *shared = NULL);


	protected:
		void UpdateModified();


	public:
		vector<cRenderWindow*> m_windows;
		vector<cDockWindow*> m_docks;
		vector<cRenderWindow*> m_poolWindow;
		vector<cDockWindow*> m_poolDock;
		vector<cRenderWindow*> m_rmWindows;
		vector<cRenderWindow*> m_addWindows;

		cRenderWindow *m_mainWindow;
		cDockWindow *m_dockTarget;
		cRenderWindow *m_dragWindow;
		eDockType::Enum m_dockSlot;
	};

}
