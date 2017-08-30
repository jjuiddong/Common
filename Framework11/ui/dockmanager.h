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
		cRenderWindow* NewRenderWindow(const StrId &title, const int width, const int height);
		void DeleteRenderWindow(cRenderWindow *wnd);
		cDockWindow* NewDockWindow();
		void DeleteDockWindow(cDockWindow *p);
		void UpdateRender(const float deltaSeconds);
		void Clear();
		void SetDragState(cRenderWindow *drag, const bool isDragStart=true);
		void SetMoveState(cRenderWindow *drag, const bool isMove=true);
		void SetDragTarget(cDockWindow *target);
		cRenderWindow* GetMainWindow();
		graphic::cRenderer* GetMainRenderer();
		bool IsDragState();
		bool IsMoveState();
		void LostDevice();
		void ResetDevice();


	protected:
		void UpdateModified();


	public:
		struct eState {
			enum Enum {
				NORMAL,
				MOVE,
				DRAG,
				DRAG_END,
		};};

		eState::Enum m_state;
		vector<cRenderWindow*> m_windows;
		vector<cRenderWindow*> m_poolWindow;
		vector<cDockWindow*> m_poolDock;
		vector<cRenderWindow*> m_rmWindows;
		vector<cRenderWindow*> m_addWindows;

		cRenderWindow *m_mainWindow;
		cDockWindow *m_dockTarget;
		cRenderWindow *m_dragWindow;
	};


	inline cRenderWindow* GetMainWindow() { return cDockManager::Get()->GetMainWindow(); }
	inline graphic::cRenderer* GetMainRenderer() { return cDockManager::Get()->GetMainRenderer(); }
}
