
#include "stdafx.h"
#include "renderwindow.h"
#include "dockwindow.h"
#include "dockmanager.h"
#include <mmsystem.h>


using namespace graphic;
using namespace framework;


cRenderWindow::cRenderWindow()
	: m_mainWindow(NULL)
	, m_hInstance(NULL)
	, m_state(eState::NORMAL)
	, m_camera("render window camera")
	, m_isVisible(true)
	, m_isRequestResetDevice(false)
	, m_dock(NULL)
	, m_sizingWindow(NULL)
	, m_isFullScreen(false)
	, m_cursorType(eDockSizingType::NONE)
	, m_resizeCursor(eResizeCursor::NONE)
	, m_captureDock(NULL)
{
}

cRenderWindow::~cRenderWindow()
{
	Clear();
}


bool cRenderWindow::Create(const HINSTANCE hInst, const bool isMainWindow, const StrId &title
	, const float width, const float height
	, cRenderWindow *mainWindow //= NULL
	, bool isTitleBar // = true
)
{
	__super::create(sf::VideoMode((int)width, (int)height), title.c_str(),
		(isTitleBar ? sf::Style::Default : sf::Style::None));

	m_hInstance = hInst;
	ID3D11Device *mainDevice = (mainWindow) ? mainWindow->m_renderer.GetDevice() : NULL;
	ID3D11DeviceContext *devContext = (mainWindow) ? mainWindow->m_renderer.GetDevContext() : NULL;
	if (isMainWindow)
	{
		if (!m_renderer.CreateDirectX(false, getSystemHandle(), width, height))
			return false;
	}
	else
	{
		if (!m_renderer.CreateDirectXSubRenderer(false, getSystemHandle(), width, height, mainDevice, devContext))
			return false;
	}

	m_title = StrId(" - ") + title + StrId(" - ");
	m_camera.SetCamera(Vector3(10, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_camera.SetProjection(MATH_PI / 4.f, (float)width / (float)height, 1.f, 10000.0f);
	m_camera.SetViewPort((float)width, (float)height);
	m_camera.Bind(m_renderer);

	m_light.Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(1.f, 1.f, 1.f, 1), 
		Vector4(1.f, 1.f, 1.f, 1),
		Vector4(1.f, 1.f, 1.f, 1));
	m_light.SetPosition(Vector3(-30000, 30000, -30000));
	m_light.SetDirection(Vector3(1, -1, 1).Normal());

	m_mainWindow = mainWindow;

	//m_renderer.SetNormalizeNormals(true);
	//m_renderer.SetLightEnable(0, true);
	m_light.Bind(m_renderer);

	m_gui.Init(getSystemHandle(), m_renderer.GetDevice(), m_renderer.GetDevContext(), ((mainWindow) ? mainWindow->m_gui.m_FontAtlas : NULL));

	m_gui.SetContext();
	ImGuiIO& io = ImGui::GetIO();
	if (!mainWindow)
	{
		const float fontSize = 14;
		//const float fontSize = 18;
		io.Fonts->AddFontFromFileTTF("../Media/extra_fonts/³ª´®°íµñBold.ttf", fontSize, NULL, io.Fonts->GetGlyphRangesKorean());
		io.ImeWindowHandle = (HWND)this->getSystemHandle();

		//ImVector<ImWchar> ranges;
		//ImFontAtlas::GlyphRangesBuilder builder;
		//builder.AddText("Hello world");                        // Add a string (here "Hello world" contains 7 unique characters)
		//builder.AddChar(0x7262);                               // Add a specific character
		//builder.AddRanges(io.Fonts->GetGlyphRangesKorean()); // Add one of the default ranges
		//builder.BuildRanges(&ranges);                          // Build the final result (ordered ranges with all the unique characters submitted)
		////io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels, NULL, ranges.Data);
		//io.Fonts->AddFontFromFileTTF("../Media/extra_fonts/³ª´®°íµñBold.ttf", 18, NULL, ranges.Data);
	}

	return true;
}


// return value : return false if close window
bool cRenderWindow::TranslateEvent()
{
	m_gui.SetContext();
	m_input.NewFrame();

	sf::Event evt;
	while (pollEvent(evt))
	{
		if (evt.type == sf::Event::Closed)
		{
			close();
			return false;
		}
		else
		{
			DefaultEventProc(evt);
		}
	}

	return true;
}


void cRenderWindow::Update(const float deltaSeconds)
{
	RET(!isOpen());
	RET(!m_isVisible);

	if (m_isRequestResetDevice)
	{
		ChangeDevice();
		m_isRequestResetDevice = false;
	}

	// Check Resize End
	if (eState::WINDOW_RESIZE == m_state)
	{
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) // Mouse Left Button Up, state change bug fix
			ChangeState(eState::NORMAL, 8);
	}

	// Maximize Window HotKey
	// Win + Up or LControl + Up 
	if ((GetFocus() == getSystemHandle()) && 
		((GetAsyncKeyState(VK_LCONTROL) & 0x8000) || (GetAsyncKeyState(VK_LWIN) & 0x8000)) 
		&& (GetAsyncKeyState(VK_UP) & 0x8000))
	{
		WINDOWPLACEMENT wndPl;
		GetWindowPlacement(getSystemHandle(), &wndPl); // Toggle Maximize or Restore
		if (wndPl.showCmd != SW_MAXIMIZE)
		{
			ShowWindow(getSystemHandle(), SW_MAXIMIZE);
			m_isFullScreen = true;
		}
	}

	// Restore Window HotKey
	// Win + Down or LControl + Down
	if ((GetFocus() == getSystemHandle()) && ((GetAsyncKeyState(VK_LWIN) & 0x8000))
		&& (GetAsyncKeyState(VK_DOWN) & 0x8000))
	{
		// Treaky Code, change m_isFullScreen Flag
		// Best Work is Restore Event Handling
		m_isFullScreen = false;
	}

	if (m_dock)
		m_dock->Update(deltaSeconds);

	MouseProc(deltaSeconds);

	m_renderer.Update(deltaSeconds);

	OnUpdate(deltaSeconds);
}


void cRenderWindow::MouseProc(const float deltaSeconds)
{
	const Vector2 pos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	switch (m_state)
	{
	case eState::NORMAL:
	{
		auto result = UpdateCursor();
		if (ImGui::IsMouseDown(0))
		{
			if (result.first)
			{ // Resize Render Window
				ChangeState(eState::WINDOW_RESIZE);
				GetCursorPos(&m_resizeClickPos);
			}
			else
			{ // Reize Docking Window 
				if (result.second)
				{
					ChangeState(eState::DOCK_SIZE);
					m_mousePos = pos;
					m_sizingWindow = result.second;
				}
				else
				{
					ChangeState(eState::NORMAL_DOWN);
				}
			}
		}
	}
	break;

	case eState::NORMAL_DOWN:
	case eState::NORMAL_DOWN_ETC:
	case eState::NORMAL_DOWN_TITLE:
	{
		if (ImGui::IsMouseReleased(0))
		{
			ChangeState(eState::NORMAL, 1);
			m_sizingWindow = NULL;
		}
	}
	break;

	case eState::WINDOW_RESIZE:
		Resize();
		break;

	case eState::DOCK_SIZE:
	{
		if (m_sizingWindow)
		{
			Vector2 delta = pos - m_mousePos;
			m_sizingWindow->CalcResizeWindow(eDockResize::DOCK_WINDOW, 
				(m_sizingWindow->GetDockSizingType() == eDockSizingType::VERTICAL) ? (int)delta.y : (int)delta.x);
			m_mousePos = pos;
		}

		if (ImGui::IsMouseReleased(0))
		{
			ChangeState(eState::NORMAL, 2);
			if (m_sizingWindow)
				m_sizingWindow->ResizeEnd(eDockResize::DOCK_WINDOW, m_sizingWindow->m_rect);
			m_sizingWindow = NULL;
		}
	}
	break;

	case eState::DRAG:
	{
		POINT mousePos;
		GetCursorPos(&mousePos);
		setPosition(sf::Vector2i((int)mousePos.x - 30, (int)mousePos.y - 60));

		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) // state change bug fix
			ChangeState(eState::NORMAL, 3);
	}
	break;

	case eState::MOVE:
		break;

	case eState::DRAG_BIND:
		break;

	default: assert(0);
	}
}


void cRenderWindow::Resize()
{
	POINT pos;
	GetCursorPos(&pos);
	const POINT delta = { pos.x - m_resizeClickPos.x, pos.y - m_resizeClickPos.y };

	if ((delta.x == 0) && (delta.y == 0))
		return;

	m_resizeClickPos = pos;
	sf::Vector2u winSize = getSize();
	sf::Vector2i winPos = getPosition();

	switch (m_resizeCursor)
	{
	case eResizeCursor::LEFT:
		winSize.x -= delta.x;
		winPos.x += delta.x;
		break;
	case eResizeCursor::RIGHT:
		winSize.x += delta.x;
		break;
	case eResizeCursor::TOP:
		winSize.y -= delta.y;
		winPos.y += delta.y;
		break;
	case eResizeCursor::BOTTOM:
		winSize.y += delta.y;
		break;
	case eResizeCursor::LEFT_TOP:
		winSize.x -= delta.x;
		winPos.x += delta.x;
		winSize.y -= delta.y;
		winPos.y += delta.y;
		break;
	case eResizeCursor::RIGHT_BOTTOM:
		winSize.x += delta.x;
		winSize.y += delta.y;
		break;
	case eResizeCursor::RIGHT_TOP:
		winSize.x += delta.x;
		winSize.y -= delta.y;
		winPos.y += delta.y;
		break;
	case eResizeCursor::LEFT_BOTTOM:
		winSize.x -= delta.x;
		winPos.x += delta.x;
		winSize.y += delta.y;
		break;
	default: break; // NONE
	}

	setSize(sf::Vector2u(winSize.x, winSize.y));
	setPosition(sf::Vector2i(winPos.x, winPos.y));

	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))  // Mouse Left Button Up
	{
		ChangeState(eState::NORMAL, 4);
	}
}


void cRenderWindow::ChangeState( const eState::Enum nextState
	, const int option //=0
)
{
	if (m_state == nextState)
		return;

	switch (m_state)
	{
	case eState::NORMAL:
	case eState::NORMAL_DOWN:
	case eState::NORMAL_DOWN_ETC:
	case eState::NORMAL_DOWN_TITLE:
		break;

	case eState::WINDOW_RESIZE:
		if (m_renderer.CheckResetDevice())
		{
			sf::Vector2u size = getSize();
			ChangeDevice(size.x, size.y);
			if (m_dock)
				m_dock->ResizeEnd(eDockResize::RENDER_WINDOW, m_dock->m_rect);
		}
		break;

	case eState::DOCK_SIZE:
		break;

	case eState::DRAG:
		setAlpha(1.f);
		cDockManager::Get()->SetDragState(this, false);
		break;

	case eState::MOVE:
		cDockManager::Get()->SetMoveState(this, false);
		break;

	case eState::DRAG_BIND:
		if (m_dock)
			m_dock->SetBindState(false);
		break;
	}

	switch (nextState)
	{
	case eState::NORMAL:
	case eState::NORMAL_DOWN:
	case eState::NORMAL_DOWN_ETC:
	case eState::NORMAL_DOWN_TITLE:
	case eState::WINDOW_RESIZE:
	case eState::DOCK_SIZE:
		break;

	case eState::DRAG:
		if (m_isFullScreen)
			ShowWindow(getSystemHandle(), SW_RESTORE);

		setAlpha(0.3f);
		cDockManager::Get()->SetDragState(this, true);
		break;

	case eState::MOVE:
		cDockManager::Get()->SetMoveState(this, true);
		break;

	case eState::DRAG_BIND:
		if (m_dock)
			m_dock->SetBindState(true);
		break;
	}

	//dbg::Log("RenderWindow::ChangeState, window=%s, cur=%s, next=%s, opt=%d\n"
	//	, m_title.c_str(), GetStateString(m_state), GetStateString(nextState), option);

	m_state = nextState;
}


// return first == true  --> window resize
//					== false --> dock window resize
std::pair<bool, cDockWindow*> cRenderWindow::UpdateCursor()
{
	const Vector2 pos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	eDockSizingType::Enum cursorType = eDockSizingType::NONE;
	eResizeCursor::Enum resizeCursor = eResizeCursor::NONE;

	bool isWindowResize = true;
	cDockWindow *sizerWnd = NULL;

	// Window ReSize check
	const Vector2 size((float)getSize().x, (float)getSize().y);
	const Vector2 delta = size - pos;
	
	if (!m_isFullScreen) // Only Check Normal Window
	{
		if ((delta.x < 10) && (delta.y < 10)) resizeCursor = eResizeCursor::RIGHT_BOTTOM;
		else if((delta.x > size.x-10) && (delta.y > size.y - 10)) resizeCursor = eResizeCursor::LEFT_TOP;
		else if ((delta.x < 10) && (delta.y > size.y - 10)) resizeCursor = eResizeCursor::RIGHT_TOP;
		else if ((delta.x > size.x - 10) && (delta.y < 10)) resizeCursor = eResizeCursor::LEFT_BOTTOM;
		else if (delta.x < 10) resizeCursor = eResizeCursor::RIGHT;
		else if (delta.x > size.x-10) resizeCursor = eResizeCursor::LEFT;
		else if (delta.y < 10) resizeCursor = eResizeCursor::BOTTOM;
		else if (delta.y > size.y - 10) resizeCursor = eResizeCursor::TOP;
	}

	if (eResizeCursor::NONE == resizeCursor)
	{
		isWindowResize = false;
		sizerWnd = GetSizerTargetWindow(pos);
		if (sizerWnd)
			cursorType = sizerWnd->GetDockSizingType();
	}
	
	m_resizeCursor = resizeCursor;
	switch (resizeCursor)
	{
	case eResizeCursor::LEFT:
	case eResizeCursor::RIGHT:
		cursorType = eDockSizingType::HORIZONTAL;
		break;
	case eResizeCursor::TOP:
	case eResizeCursor::BOTTOM:
		cursorType = eDockSizingType::VERTICAL;
		break;
	case eResizeCursor::LEFT_TOP:
	case eResizeCursor::RIGHT_BOTTOM:
		cursorType = eDockSizingType::TOPLEFT_BTTOMRIGHT;
		break;
	case eResizeCursor::RIGHT_TOP:
	case eResizeCursor::LEFT_BOTTOM:
		cursorType = eDockSizingType::BOTTOMLEFT_TOPRIGHT;
		break;
	default: break; // NONE
	}

	if (m_cursorType != cursorType)
	{
		m_cursorType = cursorType;

		switch (cursorType)
		{
		case eDockSizingType::HORIZONTAL: setMouseCursor(sf::Window::SizeHorizontal); break;
		case eDockSizingType::VERTICAL: setMouseCursor(sf::Window::SizeVertical); break;
		case eDockSizingType::TOPLEFT_BTTOMRIGHT: setMouseCursor(sf::Window::SizeTopLeftBottomRight); break;
		case eDockSizingType::BOTTOMLEFT_TOPRIGHT: setMouseCursor(sf::Window::SizeBottomLeftTopRight); break;
		default: setMouseCursor(sf::Window::Arrow); break;
		}
	}

	return{ isWindowResize, sizerWnd };
}


void cRenderWindow::Render(const float deltaSeconds)
{
	RET(!isOpen());
	RET(!m_isVisible);
	if (eState::WINDOW_RESIZE == m_state)
	{ // Only Refresh and Return
		m_renderer.Present();
		return;
	}

	m_gui.SetContext();
	m_gui.NewFrame();

	PreRender(deltaSeconds);

	if (m_dock)
	{
		const sf::Vector2u size = getSize();
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)));

		RenderTitleBar();

		const ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoFocusOnAppearing
			;
		ImGui::Begin("", NULL, flags);
		m_dock->RenderDock(deltaSeconds);
		ImGui::End();
	}

	m_camera.Bind(m_renderer);
	m_light.Bind(m_renderer);

	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();
		
		OnRender(deltaSeconds);

		m_gui.Render();

		m_renderer.EndScene();
		m_renderer.Present();
	}
	else
	{
		ChangeDevice(0, 0, true);
	}

	PostRender(deltaSeconds);
}


void cRenderWindow::RenderTitleBar()
{
	const ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoFocusOnAppearing
		;

	ImGui::Begin("", NULL, flags);
	
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 3.0f));
	const ImVec4 childBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];

	ImGui::PushStyleColor(ImGuiCol_Button, childBg);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, childBg);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, childBg);

	ImGui::SetCursorPos(ImVec2(0, 1));
	ImGui::Button(m_title.c_str(), ImVec2((float)getSize().x-150, TITLEBAR_HEIGHT));

	// TitleBar Click?
	if (ImGui::IsMouseDown(0) 
		&& ((ImGui::IsItemHovered() && (eState::NORMAL_DOWN == m_state))
			|| cDockManager::Get()->IsMoveState())
		)
	{
		if (ImGui::IsMouseDoubleClicked(0)) // Double Click, Maximize Window
		{
			ChangeState(eState::NORMAL, 5);
			ImGui::GetIO().MouseDown[0] = false; // maximize window move bug fix

			WINDOWPLACEMENT wndPl;
			GetWindowPlacement(getSystemHandle(), &wndPl);
			ShowWindow(getSystemHandle(), (wndPl.showCmd == SW_MAXIMIZE) ? SW_RESTORE : SW_MAXIMIZE);
			m_isFullScreen = (wndPl.showCmd != SW_MAXIMIZE);
		}
		else if (!m_isFullScreen)
		{
			ChangeState(eState::MOVE);

			// TitleBar Click and Move
			POINT mousePos;
			GetCursorPos(&mousePos);
			ScreenToClient(getSystemHandle(), &mousePos);
			sf::Vector2i windowPos = getPosition();
			const Vector2 delta = Vector2((float)mousePos.x, (float)mousePos.y) - m_clickPos;
			windowPos += sf::Vector2i((int)delta.x, (int)delta.y);
			setPosition(windowPos);
		}
	}
	else
	{
		// TitleBar Click Release?
		if (IsMoveState() && !ImGui::IsMouseDown(0))
		{
			ChangeState(eState::NORMAL, 6);
		}
		else if (eState::NORMAL_DOWN == m_state)
		{
			ChangeState(eState::NORMAL_DOWN_ETC);
		}
	}

	ImGui::PopStyleColor(3);

	ImGui::SameLine(); 
	if (ImGui::Button("-", ImVec2(TITLEBAR_HEIGHT, TITLEBAR_HEIGHT))) // Minimize Button
	{
		ShowWindow(getSystemHandle(), SW_MINIMIZE);
	}

	ImGui::SameLine(); 
	if (ImGui::Button("+", ImVec2(TITLEBAR_HEIGHT, TITLEBAR_HEIGHT))) // Maximize Button
	{
		WINDOWPLACEMENT wndPl;
		GetWindowPlacement(getSystemHandle(), &wndPl); // Toggle Maximize or Restore
		ShowWindow(getSystemHandle(), (wndPl.showCmd==SW_MAXIMIZE)? SW_RESTORE : SW_MAXIMIZE);
		m_isFullScreen = (wndPl.showCmd != SW_MAXIMIZE);
	}

	ImGui::SameLine(); 
	if (ImGui::Button("X", ImVec2(TITLEBAR_HEIGHT, TITLEBAR_HEIGHT))) // Close Button
	{
		close();
	}

	ImGui::PopStyleVar(1);
	ImGui::End();
}


void cRenderWindow::PreRender(const float deltaSeconds)
{
	OnPreRender(deltaSeconds);

	if (m_dock)
		m_dock->PreRender(deltaSeconds);
}


void cRenderWindow::PostRender(const float deltaSeconds)
{
	if (m_dock)
		m_dock->PostRender(deltaSeconds);

	OnPostRender(deltaSeconds);
}


void cRenderWindow::LostDevice()
{
	m_gui.InvalidateDeviceObjects();

	if (m_dock)
		m_dock->LostDevice();

	OnLostDevice();
}


void cRenderWindow::ResetDevice()
{
	const float width = m_renderer.m_viewPort.m_vp.Width;
	const float height = m_renderer.m_viewPort.m_vp.Height;

	m_camera.SetViewPort(width, height);
	m_gui.CreateDeviceObjects();

	if (m_dock)
		m_dock->ResetDevice();

	OnResetDevice();
}


void cRenderWindow::DefaultEventProc(const sf::Event &evt)
{
	m_input.MouseProc(evt);

	OnEventProc(evt);

	if (m_captureDock)
		m_captureDock->DefaultEventProc(evt);
	else if (m_dock)
		m_dock->DefaultEventProc(evt);

	ImGuiIO& io = ImGui::GetIO();
	switch (evt.type)
	{
	case sf::Event::KeyPressed:
		if (evt.key.code < 256)
			io.KeysDown[KeyboardToAscii(evt.key.code)] = 1;
		break;

	case sf::Event::KeyReleased:
		if (evt.key.code < 256)
			io.KeysDown[KeyboardToAscii(evt.key.code)] = 0;
		break;

	case sf::Event::TextEntered:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (evt.text.unicode> 0 && evt.text.unicode< 0x10000)
			io.AddInputCharacter((unsigned short)evt.text.unicode);
		break;

	case sf::Event::MouseMoved:
		io.MousePos.x = (float)evt.mouseMove.x;
		io.MousePos.y = (float)evt.mouseMove.y;
		break;

	case sf::Event::MouseButtonPressed:
		switch (evt.mouseButton.button)
		{
		case sf::Mouse::Left: {
			m_clickPos = Vector2((float)evt.mouseButton.x, (float)evt.mouseButton.y);
			io.MouseDown[0] = true;
		}
			break;
		case sf::Mouse::Right: io.MouseDown[1] = true; break;
		case sf::Mouse::Middle: io.MouseDown[2] = true; break;
		}
		break;

	case sf::Event::MouseButtonReleased:
		switch (evt.mouseButton.button)
		{
		case sf::Mouse::Left: io.MouseDown[0] = false; break;
		case sf::Mouse::Right: io.MouseDown[1] = false; break;
		case sf::Mouse::Middle: io.MouseDown[2] = false; break;
		}
		break;

	case sf::Event::MouseWheelScrolled:
		io.MouseWheel += (evt.mouseWheelScroll.delta) >= 0 ? +1.0f : -1.0f;
		break;

	case sf::Event::Resized:
	{
		// Special Size Event Process
		if ((eState::WINDOW_RESIZE != m_state) && m_renderer.CheckResetDevice())
		{
			ChangeDevice(evt.size.width, evt.size.height);

			if (m_dock)
				m_dock->ResizeEnd(eDockResize::RENDER_WINDOW, m_dock->m_rect);
		}
	}
	break;
	}
}


unsigned char cRenderWindow::KeyboardToAscii(const sf::Keyboard::Key key)
{
	switch (key)
	{
	case sf::Keyboard::BackSpace: return 0x08;
	case sf::Keyboard::Delete: return 0x2e;
	case sf::Keyboard::Left: return 0x25;
	case sf::Keyboard::Up: return 0x26;
	case sf::Keyboard::Right: return 0x27;
	case sf::Keyboard::Down: return 0x28;
	case sf::Keyboard::Home: return 0x24;
	case sf::Keyboard::End: return 0x23;
	case sf::Keyboard::Return: return VK_RETURN;
	case sf::Keyboard::V: return 'V'; // copy and paste
	case sf::Keyboard::C: return 'C'; // copy and paste
	case sf::Keyboard::LSystem: return VK_LWIN;
	case sf::Keyboard::RSystem: return VK_RWIN;
	case sf::Keyboard::LControl: return VK_LCONTROL;
	case sf::Keyboard::RControl: return VK_RCONTROL;
	default:
		return 0;
	}
}


cDockWindow* cRenderWindow::GetSizerTargetWindow(const Vector2 &mousePt)
{
	RETV(!m_dock, false);

	queue<cDockWindow*> q;
	q.push(m_dock);

	while (!q.empty())
	{
		cDockWindow *wnd = q.front();
		q.pop();

		if (wnd->m_state != eDockState::VIRTUAL)
			continue;

		if (wnd->IsInSizerSpace(mousePt))
			return wnd;

		if (wnd->m_lower)
			q.push(wnd->m_lower);
		if (wnd->m_upper)
			q.push(wnd->m_upper);
	}

	return NULL;
}


void cRenderWindow::SetDragState()
{
	ChangeState(eState::DRAG);
}


void cRenderWindow::SetDragBindState()
{
	ChangeState(eState::DRAG_BIND);
}


void cRenderWindow::SetFinishDragBindState()
{
	ChangeState(eState::NORMAL, 7);
}


bool cRenderWindow::IsDragState()
{
	return m_state == eState::DRAG;
}


bool cRenderWindow::IsMoveState()
{
	return m_state == eState::MOVE;
}


void cRenderWindow::Sleep()
{
	setVisible(false);
}


void cRenderWindow::WakeUp(const StrId &title, const float width, const float height)
{
	ChangeState(eState::NORMAL, 9);

	m_title = StrId(" - ") + title + StrId(" - ");
	setTitle(title.c_str());
	setSize(sf::Vector2u((u_int)width, (u_int)height));
	m_isFullScreen = false;
	setVisible(true);
}


void cRenderWindow::ChangeDevice(
	const int width //=0
	, const int height //=0
	, const bool forceReset //=false
)
{
	LostDevice();

	const bool restResource = (cDockManager::Get()->m_mainWindow == this);
	m_renderer.ResetDevice(0, 0, forceReset, restResource);

	const sRectf rect(0, TITLEBAR_HEIGHT2, (float)((width==0)? getSize().x : width), (float)((height==0)? getSize().y : height));
	if (m_dock)
		m_dock->CalcResizeWindow(eDockResize::RENDER_WINDOW, rect);

	ResetDevice();
}


void cRenderWindow::RequestResetDeviceNextFrame()
{
	m_isRequestResetDevice = true;
}


void cRenderWindow::SetCapture(cDockWindow *dock)
{
	m_captureDock = dock;
	::SetCapture(getSystemHandle());
}


cDockWindow* cRenderWindow::GetCapture()
{
	return m_captureDock;
}


void cRenderWindow::ReleaseCapture()
{
	m_captureDock = NULL;
	::ReleaseCapture();
}


void cRenderWindow::SetIcon(int id)
{
	SendMessage(getSystemHandle(), WM_SETICON, 0
		, reinterpret_cast<LPARAM>(LoadIcon(GetModuleHandle(nullptr)
			, MAKEINTRESOURCE(id))));
}


void cRenderWindow::Shutdown()
{
	OnShutdown();
}


void cRenderWindow::Clear()
{
	m_gui.Shutdown();

	// Delete All Docking Window
	SAFE_DELETE(m_dock);
}


const char* cRenderWindow::GetStateString(const eState::Enum state)
{
	switch (state)
	{
	case eState::NORMAL: return "Normal";
	case eState::NORMAL_DOWN: return "Normal_Down";
	case eState::NORMAL_DOWN_TITLE: return "Normal_Title";
	case eState::NORMAL_DOWN_ETC: return "Normal_Etc";
	case eState::WINDOW_RESIZE: return "Window_Resize";
	case eState::DOCK_SIZE: return "Resize";
	case eState::DRAG: return "Drag";
	case eState::MOVE: return "Move";
	case eState::DRAG_BIND: return "Drag_Bind";
	default: assert(0); return "None";
	}
}
