//
// 2017-04-03, jjuiddong
// DX Rendering Window
//
#pragma once

#include "dock.h"


namespace framework
{

	class cDockWindow;
	class cRenderWindow : public sf::Window
	{
	public:
		cRenderWindow();
		virtual ~cRenderWindow();

		virtual bool Create(const string &title, const int width, const int height, graphic::cRenderer *shared=NULL);
		virtual void Update(const float deltaSeconds);
		virtual void Render(const float deltaSeconds);
		virtual void DragAndDrop(const float deltaSeconds);
		virtual void OnUpdate(const float deltaSeconds) {}
		virtual void OnRender(const float deltaSeconds) {}
		virtual void OnPreRender(const float deltaSeconds) {}
		virtual void OnPostRender(const float deltaSeconds) {}
		virtual bool TranslateEvent();
		virtual void LostDevice();
		virtual void ResetDevice(graphic::cRenderer *shared=NULL);
		cDockWindow* GetSizerTargetWindow(const Vector2 &mousePt);
		void Sleep();
		void WakeUp(const string &title, const int width, const int height);
		void SetDragState();
		void SetDragBindState();
		void SetFinishDragBindState();
		bool IsDragState();


	protected:
		virtual void DefaultEventProc(const sf::Event &evt);
		cDockWindow* UpdateCursor();
		virtual void OnLostDevice() {}
		virtual void OnResetDevice(graphic::cRenderer *shared) {}


	public:
		struct eState {
			enum Enum {
				NORMAL,
				SIZE,
				DRAG,
				DRAG_BIND,
			};
		};

		eState::Enum m_state;
		bool m_isVisible;
		bool m_isDrag;
		graphic::cCamera m_camera;
		graphic::cLight m_light;
		graphic::cRenderer m_renderer;
		graphic::cRenderer *m_sharedRenderer;
		cImGui m_gui;
		graphic::cTexture m_texture;
		graphic::cSurface2 m_surf;
		cDockWindow *m_dock;

		Vector2 m_ptMouse;
		cDockWindow *m_sizingWindow;
		eDockSizingType::Enum m_cursorType;

		bool m_isThread;
		bool m_isThreadLoop;
		std::thread m_thread;

		static int s_adapter;
	};

}
