//
// 2017-04-03, jjuiddong
// DX Rendering Window
//
#pragma once

#include "dock.h"


namespace framework
{
	using namespace common;

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

		virtual bool Create(const HINSTANCE hInst, const bool isMainWindow
			, const StrId &title, const float width, const float height
			, cRenderWindow *mainWindow = NULL, bool isTitleBar=true);
		virtual void Update(const float deltaSeconds);
		virtual void PreRender(const float deltaSeconds);
		virtual void PostRender(const float deltaSeconds);
		virtual void Render(const float deltaSeconds);
		virtual bool TranslateEvent();
		virtual void LostDevice();
		virtual void ResetDevice();
		virtual void Shutdown();
		cDockWindow* GetSizerTargetWindow(const Vector2 &mousePt);
		void RequestResetDeviceNextFrame();
		void Sleep();
		void WakeUp(const StrId &title, const float width, const float height);
		void SetDragState();
		void SetDragBindState();
		void SetFinishDragBindState();
		bool IsDragState();
		bool IsMoveState();
		void SetCapture(cDockWindow *dock);
		cDockWindow* GetCapture();
		void ReleaseCapture();
		void SetIcon(int id);
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
		unsigned char KeyboardToAscii(const sf::Keyboard::Key key);

		virtual void OnUpdate(const float deltaSeconds) {}
		virtual void OnRender(const float deltaSeconds) {}
		virtual void OnPreRender(const float deltaSeconds) {}
		virtual void OnPostRender(const float deltaSeconds) {}
		virtual void OnEventProc(const sf::Event &evt) {}
		virtual void OnLostDevice() {}
		virtual void OnResetDevice() {}
		virtual void OnShutdown() {}


	public:
		HINSTANCE m_hInstance;
		graphic::cRenderer m_renderer;
		eState::Enum m_state;
		StrId m_title;
		cImGui m_gui;
		graphic::cCamera3D m_camera;
		graphic::cLight m_light;
		cRenderWindow *m_mainWindow; // reference
		cDockWindow *m_dock;
		cDockWindow *m_captureDock;
		cInputManager m_input;

		Vector2 m_mousePos;
		Vector2 m_clickPos;
		POINT m_resizeClickPos; // mouse screen position
		cDockWindow *m_sizingWindow;
		eDockSizingType::Enum m_cursorType;
		eResizeCursor::Enum m_resizeCursor;

		bool m_isVisible;
		bool m_isFullScreen;
		bool m_isRequestResetDevice;
		sf::Vector2u m_resetSize;
	};

}
