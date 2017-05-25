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
		struct eState {
			enum Enum {
				NORMAL,
				NORMAL_DOWN, // docking, resize bugfix
				NORMAL_DOWN_TITLE, // titlebar click
				NORMAL_DOWN_ETC, // mouse down, exceptra
				WINDOW_RESIZE,
				DOCK_SIZE, // docking window sizing
				DRAG,
				MOVE,
				DRAG_BIND,
			};
		};

		struct eResizeCursor {
			enum Enum {
				NONE,
				LEFT,
				TOP,
				RIGHT,
				BOTTOM,
				LEFT_TOP,
				RIGHT_TOP,
				LEFT_BOTTOM,
				RIGHT_BOTTOM,
			};
		};

		cRenderWindow();
		virtual ~cRenderWindow();

		virtual bool Create(const string &title, const int width, const int height
			, graphic::cRenderer *shared=NULL, bool isTitleBar= true);
		virtual void Update(const float deltaSeconds);
		virtual void PreRender(const float deltaSeconds);
		virtual void PostRender(const float deltaSeconds);
		virtual void Render(const float deltaSeconds);
		virtual bool TranslateEvent();
		virtual void LostDevice();
		virtual void ResetDevice(graphic::cRenderer *shared=NULL);
		cDockWindow* GetSizerTargetWindow(const Vector2 &mousePt);
		void RequestResetDeviceNextFrame();
		void Sleep();
		void WakeUp(const string &title, const int width, const int height);
		void SetDragState();
		void SetDragBindState();
		void SetFinishDragBindState();
		bool IsDragState();
		bool IsMoveState();
		void Clear();


	protected:
		void RenderTitleBar();
		void Resize();
		void ChangeState(const eState::Enum nextState, const int option=0);
		const char* GetStateString(const eState::Enum state);
		virtual void DefaultEventProc(const sf::Event &evt);
		virtual void MouseProc(const float deltaSeconds);
		std::pair<bool, cDockWindow*> UpdateCursor();
		void ChangeDevice(const int width = 0, const int height = 0, const bool forceReset=false);

		virtual void OnUpdate(const float deltaSeconds) {}
		virtual void OnRender(const float deltaSeconds) {}
		virtual void OnPreRender(const float deltaSeconds) {}
		virtual void OnPostRender(const float deltaSeconds) {}
		virtual void OnEventProc(const sf::Event &evt) {}
		virtual void OnLostDevice() {}
		virtual void OnResetDevice(graphic::cRenderer *shared) {}


	public:
		eState::Enum m_state;
		bool m_isVisible;
		bool m_isFullScreen;
		bool m_isRequestResetDevice;
		string m_title;
		graphic::cCamera m_camera;
		graphic::cLight m_light;
		graphic::cRenderer m_renderer;
		graphic::cRenderer *m_sharedRenderer;
		cImGui m_gui;
		graphic::cTexture m_backBuffer;
		graphic::cSurface3 m_sharedSurf;
		cDockWindow *m_dock;

		Vector2 m_mousePos;
		Vector2 m_clickPos;
		POINT m_resizeClickPos; // mouse screen position
		cDockWindow *m_sizingWindow;
		eDockSizingType::Enum m_cursorType;
		eResizeCursor::Enum m_resizeCursor;

		bool m_isThread;
		bool m_isThreadLoop;
		std::thread m_thread;

		static int s_adapter;
	};

}
