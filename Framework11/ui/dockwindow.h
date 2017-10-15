//
// 2017-04-05, jjuiddong
// Docking Window
//
#pragma once

#include "dock.h"


namespace framework
{
	class cRenderWindow;

	class cDockWindow
	{
	public:
		cDockWindow(const StrId &name = "");// , const sDockSizingOption &sizingOpt = defaultSizingOption);
		virtual ~cDockWindow();

		virtual bool Create(const eDockState::Enum state, const eDockSlot::Enum type,
			cRenderWindow *owner, cDockWindow *parent
			, const float windowSize=0.5f
			//, const sDockSizingOption &sizingOpt = defaultSizingOption
		);

		virtual bool Dock(const eDockSlot::Enum type,
			cDockWindow *child
			, const float windowSize=0.5f
			//, const sDockSizingOption &sizingOpt = defaultSizingOption
		);

		virtual bool Undock(const bool newWindow = true);
		virtual bool Undock(cDockWindow *dock);
		void RenderDock(const Vector2 &pos = Vector2(0, 0));
		virtual void Update(const float deltaSeconds);
		virtual void PreRender();
		virtual void PostRender();
		virtual void DefaultEventProc(const sf::Event &evt);
		virtual void RenderTab();
		virtual bool RemoveTab(cDockWindow *tab);
		virtual void ResizeEnd(const eDockResize::Enum type, const sRectf &rect);
		virtual void LostDevice();
		virtual void ResetDevice();
		void CalcWindowSize(cDockWindow *dock
			, const float floatwindowSize = 0.5f
			//, const sDockSizingOption &sizingOpt = defaultSizingOption
		);

		void CalcResizeWindow(const eDockResize::Enum type, const sRectf &rect);
		void CalcResizeWindowRate(const eDockResize::Enum type, const sRectf &rect);
		void CalcResizeWindow(const eDockResize::Enum type, const int deltaSize);
		bool IsInSizerSpace(const Vector2 &pos);
		eDockSizingType::Enum GetDockSizingType();
		void SetBindState(const bool enable = true);
		void SetCapture();
		cDockWindow* GetCapture();
		void ReleaseCapture();
		graphic::cRenderer& GetRenderer();
		framework::cInputManager& GetInput();
		void ClearConnection();
		void Clear();


	protected:
		void SetParentDockPtr(cDockWindow *dock);
		bool Merge(cDockWindow *udock);
		cDockWindow* UndockTab();
		eDockSlot::Enum render_dock_slot_preview(const ImVec2& mouse_pos, const ImVec2& cPos, const ImVec2& cSize);
		bool CheckEventTarget(const sf::Event &evt);

		virtual void OnUpdate(const float deltaSeconds) {}
		virtual void OnRender() {}
		virtual void OnPreRender() {}
		virtual void OnPostRender() {}
		virtual void OnResize(const eDockResize::Enum type, const sRectf &rect) {}
		virtual void OnResizeEnd(const eDockResize::Enum type, const sRectf &rect) {}
		virtual void OnEventProc(const sf::Event &evt) {}
		virtual void OnLostDevice() {}
		virtual void OnResetDevice() {}


	public:
		bool m_isBind;
		eDockState::Enum m_state;
		eDockSlot::Enum m_dockSlot;
		cRenderWindow *m_owner;
		cDockWindow *m_lower; // only availible for VIRTUAL state
		cDockWindow *m_upper; // only availible for VIRTUAL state
		cDockWindow *m_parent;
		vector<cDockWindow*> m_tabs;
		int m_selectTab;
		StrId m_name;
		sRectf m_rect;
		eDockSlot::Enum m_dragSlot; // using drag dock window
		graphic::cPickManager m_pickMgr;
	};

}
