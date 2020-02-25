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
				NORMAL_DOWN_ETC, // mouse down, etcetera
				WINDOW_RESIZE,
				DOCK_SIZE, // docking window sizing
				TAB_CLICK,
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
			, const common::Str128 &title, const float width, const float height
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
		cDockWindow* GetTargetWindow(const Vector2 &mousePt);
		void RequestResetDeviceNextFrame();
		void Sleep();
		void WakeUp(const common::Str128 &title, const float width, const float height);
		void SetTabClickState();
		void SetDragState();
		void SetDragBindState();
		void SetFinishDragBindState();
		bool IsDragState();
		bool IsMoveState();
		void SetActiveWindow(cDockWindow *dock);
		cDockWindow* SetActiveNextTabWindow(cDockWindow *dock);
		void SetCapture(cDockWindow *dock);
		void SetFocus(cDockWindow *dock);
		cDockWindow* GetCapture();
		cDockWindow* GetFocus();
		void ReleaseCapture();
		void SetIcon(int id);
		void Clear();


	protected:
		void RenderTitleBar();
		void RenderMenuBar();
		void Resize();
		void ChangeState(const eState::Enum nextState);
		const char* GetStateString(const eState::Enum state);
		virtual int DefaultEventProc(const sf::Event &evt);
		virtual void MouseProc(const float deltaSeconds);
		std::pair<bool, cDockWindow*> UpdateCursor();
		void ChangeDevice(const int width = 0, const int height = 0, const bool forceReset=false);
		unsigned char KeyboardToAscii(const sf::Keyboard::Key key);

		virtual void OnUpdate(const float deltaSeconds) {}
		virtual void OnRender(const float deltaSeconds) {}
		virtual void OnPreRender(const float deltaSeconds) {}
		virtual void OnPostRender(const float deltaSeconds) {}
		virtual void OnRenderTitleBar() {}
		virtual void OnRenderMenuBar() {}
		virtual void OnEventProc(const sf::Event &evt) {}
		virtual void OnLostDevice() {}
		virtual void OnResetDevice() {}
		virtual void OnShutdown() {}


	public:
		HINSTANCE m_hInstance;
		graphic::cRenderer m_renderer;
		eState::Enum m_state;
		common::Str128 m_title;
		cImGui m_gui;
		graphic::cCamera3D m_camera;
		graphic::cLight m_light;
		cRenderWindow *m_mainWindow; // reference
		cDockWindow *m_dock;
		cDockWindow *m_captureDock;
		cDockWindow *m_focusDock;
		cInputManager m_input;

		Vector2 m_mousePos;
		Vector2 m_clickPos;
		POINT m_resizeClickPos; // mouse screen position
		cDockWindow *m_sizingWindow;
		eDockSizingType::Enum m_cursorType;
		eResizeCursor::Enum m_resizeCursor;

		bool m_isVisible;
		bool m_isWindowTitleBar; //window default tilte bar, default: false
		bool m_isTitleBarOverriding; // title bar overriding, call OnRenderTitleBar
		bool m_isMenuBar; //menu bar, default: false
		bool m_isFullScreen;
		bool m_isRequestResetDevice;
		float m_titleBarHeight;
		float m_titleBarHeight2;
		sf::Vector2u m_resetSize;

		ImFont *m_fontBig;
		graphic::cTexture *m_titleBtn[4]; // reference, min-max-close-restore
	};

}
