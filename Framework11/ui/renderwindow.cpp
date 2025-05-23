
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
	, m_isWindowTitleBar(false)
	, m_isTitleBarOverriding(false)
	, m_isMenuBar(false)
	, m_isFullScreen(false)
	, m_isResizable(true)
	, m_cursorType(eDockSizingType::NONE)
	, m_resizeCursor(eResizeCursor::NONE)
	, m_captureDock(NULL)
	, m_focusDock(NULL)
	, m_titleBarHeight(TITLEBAR_HEIGHT)
	, m_titleBarHeight2(TITLEBAR_HEIGHT2)
{
}

cRenderWindow::~cRenderWindow()
{
	Clear();
}


bool cRenderWindow::Create(const HINSTANCE hInst, const bool isMainWindow, const common::Str128 &title
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

	m_title = common::Str128(" - ") + title + common::Str128(" - ");
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

	m_gui.Init(getSystemHandle(), m_renderer.GetDevice(), m_renderer.GetDevContext()
		, ((mainWindow) ? mainWindow->m_gui.m_FontAtlas : NULL));

	m_gui.SetContext();
	ImGuiIO& io = ImGui::GetIO();
	if (!mainWindow)
	{
		const float fontSize = 15;
		const float bigFontSize = 26;

		//const float fontSize = 26;
		//const float bigFontSize = 36;

		StrPath path1 = cResourceManager::Get()->GetMediaDirectory() + "extra_fonts/NanumGothicBold.ttf";
		io.Fonts->AddFontFromFileTTF(path1.c_str(), fontSize, NULL, io.Fonts->GetGlyphRangesKorean());
		m_fontBig = io.Fonts->AddFontFromFileTTF(path1.c_str(), bigFontSize, NULL, io.Fonts->GetGlyphRangesKorean());

		io.ImeWindowHandle = (HWND)this->getSystemHandle();

		//ImVector<ImWchar> ranges;
		//ImFontAtlas::GlyphRangesBuilder builder;
		//builder.AddText("Hello world");                        // Add a string (here "Hello world" contains 7 unique characters)
		//builder.AddChar(0x7262);                               // Add a specific character
		//builder.AddRanges(io.Fonts->GetGlyphRangesKorean()); // Add one of the default ranges
		//builder.BuildRanges(&ranges);                          // Build the final result (ordered ranges with all the unique characters submitted)
		////io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels, NULL, ranges.Data);
		//io.Fonts->AddFontFromFileTTF("../Media/extra_fonts/나눔고딕Bold.ttf", 18, NULL, ranges.Data);	}
	}

	m_titleBtn[0] = cResourceManager::Get()->LoadTexture(m_renderer, "minbtn.png");
	m_titleBtn[1] = cResourceManager::Get()->LoadTexture(m_renderer, "maxbtn.png");
	m_titleBtn[2] = cResourceManager::Get()->LoadTexture(m_renderer, "closebtn.png");
	m_titleBtn[3] = cResourceManager::Get()->LoadTexture(m_renderer, "restorebtn.png");

	return true;
}


// return value : return false if close window
bool cRenderWindow::TranslateEvent()
{
	m_gui.SetContext();
	m_input.NewFrame();
	//m_gui.NewFrame(); // move to Render()

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
			// 마우스 이벤트는 한프레임에 한번씩 처리
			if (DefaultEventProc(evt) > 0)
				break;
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
			ChangeState(eState::NORMAL);
	}

	// Maximize Window HotKey
	// Win + Up or LControl + Up 
	if ((::GetFocus() == getSystemHandle()) && 
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
	if ((::GetFocus() == getSystemHandle()) && ((GetAsyncKeyState(VK_LWIN) & 0x8000))
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
	// 창을 옮기거나, 도킹 중일 때는, 해당 창의 마우스 이벤트만 처리한다.
	if (cDockManager::Get()->IsDragState())
	{
		if ((m_state != eState::DRAG) && (m_state != eState::DRAG_BIND))
			return;
	}
	if (cDockManager::Get()->IsClickState())
		return;

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
				if (m_isResizable)
				{
					ChangeState(eState::WINDOW_RESIZE);
					GetCursorPos(&m_resizeClickPos);
				}
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
	case eState::TAB_CLICK:
	{
		if (ImGui::IsMouseReleased(0))
		{
			ChangeState(eState::NORMAL);
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
			ChangeState(eState::NORMAL);
			if (m_sizingWindow)
				m_sizingWindow->ResizeEnd(eDockResize::DOCK_WINDOW, m_sizingWindow->m_rect);
			m_sizingWindow = NULL;
		}
	}
	break;

	case eState::DRAG:
	{
		POINT mousePos;
		if (::GetCursorPos(&mousePos))
		{
			setPosition(sf::Vector2i((int)mousePos.x - 30, 
				(int)mousePos.y - (int)m_titleBarHeight - (int)(TAB_H/2.f)));
		}

		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) // state change bug fix
			ChangeState(eState::NORMAL);
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
		ChangeState(eState::NORMAL);
	}
}


void cRenderWindow::ChangeState( const eState::Enum nextState)
{
	if (m_state == nextState)
		return;

	switch (m_state)
	{
	case eState::NORMAL:
	case eState::NORMAL_DOWN:
	case eState::NORMAL_DOWN_ETC:
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

	case eState::TAB_CLICK:
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
	case eState::WINDOW_RESIZE:
	case eState::DOCK_SIZE:
		break;

	case eState::TAB_CLICK:
		cDockManager::Get()->SetClickState();
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
//				== false --> dock window resize
std::pair<bool, cDockWindow*> cRenderWindow::UpdateCursor()
{
	POINT tmpPos;
	::GetCursorPos(&tmpPos);
	const Vector2 pos((float)(tmpPos.x - getPosition().x), (float)(tmpPos.y - getPosition().y));

	eDockSizingType::Enum cursorType = eDockSizingType::NONE;
	eResizeCursor::Enum resizeCursor = eResizeCursor::NONE;

	bool isWindowResize = true;
	cDockWindow *sizerWnd = NULL;

	// Window ReSize check
	const Vector2 size((float)getSize().x, (float)getSize().y);
	const Vector2 delta = size - pos;
	const sRectf rect(0, 0, size.x, size.y);
	const int Gap = 10;
	if (!m_isFullScreen && rect.IsIn(delta.x, delta.y)) // Only Check Normal Window
	{
		if ((delta.x < Gap) && (delta.y < Gap)) resizeCursor = eResizeCursor::RIGHT_BOTTOM;
		else if((delta.x > size.x - Gap) && (delta.y > size.y - Gap)) resizeCursor = eResizeCursor::LEFT_TOP;
		else if ((delta.x < Gap) && (delta.y > size.y - Gap)) resizeCursor = eResizeCursor::RIGHT_TOP;
		else if ((delta.x > size.x - Gap) && (delta.y < Gap)) resizeCursor = eResizeCursor::LEFT_BOTTOM;
		else if (delta.x < Gap) resizeCursor = eResizeCursor::RIGHT;
		else if (delta.x > size.x- Gap) resizeCursor = eResizeCursor::LEFT;
		else if (delta.y < Gap) resizeCursor = eResizeCursor::BOTTOM;
		else if (delta.y > size.y - Gap) resizeCursor = eResizeCursor::TOP;
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
	m_gui.NewFrame(); // for node editor work

	RET(!isOpen());
	RET(!m_isVisible);
	if (eState::WINDOW_RESIZE == m_state)
	{ // Only Refresh and Return
		m_renderer.Present();
		return;
	}

	PreRender(deltaSeconds);

	if (m_dock)
	{
		const sf::Vector2u size = getSize();

		// titlebar
		if (!m_isWindowTitleBar && !m_isTitleBarOverriding)
			RenderTitleBar();
		else if (m_isTitleBarOverriding)
			OnRenderTitleBar();

		if (m_isMenuBar)
			RenderMenuBar();

		const ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoFocusOnAppearing
			;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)size.x, (float)size.y));
		ImGui::Begin("##renderwindow_view", NULL, flags);
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
	const sf::Vector2u size = getSize();
	const ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoFocusOnAppearing
		;

	ImGui::SetNextWindowPos(ImVec2(1, 1));
	ImGui::SetNextWindowSize(ImVec2((float)size.x - 2, m_titleBarHeight));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(100.f, TITLEBAR_HEIGHT));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 3.0f));

	StrId wndId;
	wndId.Format("##titlebar%x", this);
	ImGui::Begin(wndId.c_str(), NULL, flags);
	
	const ImVec4 childBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];

	ImGui::PushStyleColor(ImGuiCol_Button, childBg);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, childBg);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, childBg);

	ImGui::SetCursorPos(ImVec2(0, 0));
	const float systemBtnSize = (m_titleBarHeight * 3) + 28;
	ImGui::Button(m_title.c_str(), ImVec2((float)getSize().x- systemBtnSize, m_titleBarHeight));

	// TitleBar Click?
	if (ImGui::IsMouseDown(0) 
		&& ((ImGui::IsItemHovered() && (eState::NORMAL_DOWN == m_state))
			|| cDockManager::Get()->IsMoveState())
		)
	{
		if (ImGui::IsMouseDoubleClicked(0)) // Double Click, Maximize Window
		{
			ChangeState(eState::NORMAL);
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
			if (::GetCursorPos(&mousePos))
			{
				ScreenToClient(getSystemHandle(), &mousePos);
				sf::Vector2i windowPos = getPosition();
				const Vector2 delta = Vector2((float)mousePos.x, (float)mousePos.y) - m_clickPos;
				windowPos += sf::Vector2i((int)delta.x, (int)delta.y);
				setPosition(windowPos);
			}
		}
	}
	else
	{
		// TitleBar Click Release?
		if (IsMoveState() && !ImGui::IsMouseDown(0))
		{
			ChangeState(eState::NORMAL);
		}
		else if (eState::NORMAL_DOWN == m_state)
		{
			ChangeState(eState::NORMAL_DOWN_ETC);
		}
	}

	ImGui::PopStyleColor(3);


	// Title minimize, maximize, restore, close Button Color
	const float col_main_hue = 0.0f / 255.0f;
	const float col_main_sat = 0.0f / 255.0f;
	const float col_main_val = 80.0f / 255.0f;
	const ImVec4 col_main = ImColor::HSV(col_main_hue, col_main_sat, col_main_val);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(col_main.x, col_main.y, col_main.z, 0.44f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(col_main.x, col_main.y, col_main.z, 0.86f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(col_main.x, col_main.y, col_main.z, 1.00f));

	ImGui::SameLine();
	if (ImGui::ImageButton(m_titleBtn[0]->m_texSRV, ImVec2(m_titleBarHeight, m_titleBarHeight)
		, ImVec2(0,0), ImVec2(1,1), 0)) // Minimize Button
	{
		ShowWindow(getSystemHandle(), SW_MINIMIZE);
	}

	ImGui::SameLine();
	WINDOWPLACEMENT wndPl;
	GetWindowPlacement(getSystemHandle(), &wndPl); // Toggle Maximize or Restore
	if (wndPl.showCmd == SW_MAXIMIZE)
	{
		if (ImGui::ImageButton(m_titleBtn[3]->m_texSRV, ImVec2(m_titleBarHeight, m_titleBarHeight)
			, ImVec2(0, 0), ImVec2(1, 1), 0)) // Restore Button
		{
			ShowWindow(getSystemHandle(), SW_RESTORE);
			m_isFullScreen = false;
		}
	}
	else
	{
		if (ImGui::ImageButton(m_titleBtn[1]->m_texSRV, ImVec2(m_titleBarHeight, m_titleBarHeight)
			, ImVec2(0, 0), ImVec2(1, 1), 0)) // Maximize Button
		{
			ShowWindow(getSystemHandle(), SW_MAXIMIZE);
			m_isFullScreen = true;
		}
	}

	ImGui::SameLine(); 
	if (ImGui::ImageButton(m_titleBtn[2]->m_texSRV, ImVec2(m_titleBarHeight, m_titleBarHeight)
		, ImVec2(0, 0), ImVec2(1, 1), 0)) // Close Button
	{
		close();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(1);// frame padding
	ImGui::End();

	ImGui::PopStyleColor(1); // border color
	ImGui::PopStyleVar(1);// window padding, frame padding, minsize
}


// render menu
void cRenderWindow::RenderMenuBar()
{
	const float y = m_isWindowTitleBar ? 0.f : m_titleBarHeight + 1;
	ImGui::SetNextWindowPos(ImVec2(3, y));
	ImGui::SetNextWindowSize(ImVec2((float)getSize().x-5, MENUBAR_HEIGHT));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(100, MENUBAR_HEIGHT));

	const ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBackground
		| ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_MenuBar
		;
	if (ImGui::Begin("MenuBar", NULL, flags))
	{
		OnRenderMenuBar(); // override derivate class
	}
	ImGui::End();

	ImGui::PopStyleVar(1);
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


// 2019-06-06
// 터치 이벤트를 받기 위해서, 리턴값 추가
// 속도가 느릴 때, MouseButton Up/Down, Touch Down/Up이 동시에 
// 처리되어서 버튼 입력을 못받는 경우가 있다. 
// 그래서 마우스, 터치패드 입력을 받을 경우, 한 이벤트만 처리하고,
// 나머지 이벤트는 다음 프레임 때 처리하게 한다.
int cRenderWindow::DefaultEventProc(const sf::Event &evt)
{
	int mouseEvent = 0;
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
		if (evt.key.cmd < 256)
			io.KeysDown[KeyboardToAscii(evt.key.cmd)] = 1;
		break;

	case sf::Event::KeyReleased:
		if (evt.key.cmd < 256)
			io.KeysDown[KeyboardToAscii(evt.key.cmd)] = 0;
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
			io.MousePos.x = (float)evt.mouseButton.x;
			io.MousePos.y = (float)evt.mouseButton.y;
			io.MouseDown[0] = true;
		}
		break;
		case sf::Mouse::Right: io.MouseDown[1] = true; break;
		case sf::Mouse::Middle: io.MouseDown[2] = true; break;
		}
 		SetFocus(GetTargetWindow(Vector2((float)evt.mouseButton.x, (float)evt.mouseButton.y)));
		mouseEvent = 1;
		break;

	case sf::Event::MouseButtonReleased:
		switch (evt.mouseButton.button)
		{
		case sf::Mouse::Left: io.MouseDown[0] = false; break;
		case sf::Mouse::Right: io.MouseDown[1] = false; break;
		case sf::Mouse::Middle: io.MouseDown[2] = false; break;
		}
		mouseEvent = 1;
		break;

	case sf::Event::MouseWheelScrolled:
		io.MouseWheel += (evt.mouseWheelScroll.delta) >= 0 ? +1.0f : -1.0f;
		break;

	case sf::Event::TouchBegan:
	{
		m_clickPos = Vector2((float)evt.touch.x, (float)evt.touch.y);
		io.MouseDown[0] = true;
		io.MousePos.x = (float)evt.touch.x;
		io.MousePos.y = (float)evt.touch.y;
	}
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

	return mouseEvent;
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


// mousePt가 Docking Window 크기조절 영역에 위치할 때, 해당 DockWindow를 리턴한다.
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


// mousePt가 위치해있는 Dock Window를 리턴한다.
cDockWindow* cRenderWindow::GetTargetWindow(const Vector2 &mousePt)
{
	RETV(!m_dock, false);

	queue<cDockWindow*> q;
	q.push(m_dock);

	while (!q.empty())
	{
		cDockWindow *wnd = q.front();
		q.pop();

		if (wnd->m_state == eDockState::VIRTUAL)
		{
			if (wnd->m_lower)
				q.push(wnd->m_lower);
			if (wnd->m_upper)
				q.push(wnd->m_upper);
		}
		else
		{
			// convert screen pos -> docking window pos
			Vector2 pos((float)mousePt.x, (float)mousePt.y);
			pos.x -= wnd->m_rect.left;
			pos.y -= wnd->m_rect.top;

			if (0 == wnd->m_selectTab)
			{
				if (wnd->IsInWindow(pos))
					return wnd;
			}
			else if((int)wnd->m_tabs.size() > wnd->m_selectTab - 1)
			{
				cDockWindow *p = wnd->m_tabs[wnd->m_selectTab - 1];
				if (p->IsInWindow(pos))
					return p;
			}
		}
	}

	return NULL;
}


void cRenderWindow::SetTabClickState()
{
	ChangeState(eState::TAB_CLICK);
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
	ChangeState(eState::NORMAL);
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


void cRenderWindow::WakeUp(const common::Str128 &title, const float width, const float height)
{
	ChangeState(eState::NORMAL);

	m_title = common::Str128(" - ") + title + common::Str128(" - ");
	setTitle(title.c_str());
	setSize(sf::Vector2u((uint)width, (uint)height));
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

	sRectf rect;
	if (m_isWindowTitleBar)
	{
		rect = sRectf(0, ((m_isMenuBar) ? MENUBAR_HEIGHT2 : 0.f)
			, (float)((width == 0) ? getSize().x : width)
			, (float)((height == 0) ? getSize().y - 5 : height - 5));
	}
	else
	{
		rect = sRectf(0, m_titleBarHeight2 
				+ ((m_isMenuBar)? MENUBAR_HEIGHT2 : 0.f)
			, (float)((width == 0) ? getSize().x : width)
			, (float)((height == 0) ? getSize().y : height));
	}

	if (m_dock)
		m_dock->CalcResizeWindow(eDockResize::RENDER_WINDOW, rect);

	ResetDevice();
}


void cRenderWindow::RequestResetDeviceNextFrame()
{
	m_isRequestResetDevice = true;
}


// show dock window
void cRenderWindow::SetActiveWindow(cDockWindow *dock)
{
	RET(!dock);
	RET(!dock->m_parent); // leaf node?

	// todo: recursive check
	// if tab state, set current tab index
	cDockWindow *tabWnd = dock->m_tabs.empty() ? dock->m_parent : dock;
	RET(tabWnd->m_tabs.empty());

	if (tabWnd == dock)
	{
		tabWnd->m_selectTab = 0;
		return; // success
	}
	
	int tabIdx = -1;
	for (uint i = 0; i < tabWnd->m_tabs.size(); ++i)
	{
		if (dock == tabWnd->m_tabs[i])
		{
			tabIdx = (int)i;
			break;
		}
	}
	if (tabIdx < 0)
		return; // error occurred!

	tabWnd->m_selectTab = tabIdx + 1;
}


// show next tab window
cDockWindow* cRenderWindow::SetActiveNextTabWindow(cDockWindow *dock)
{
	RETV(!dock, nullptr);
	RETV(!dock->m_parent, nullptr); // leaf node?

	// todo: recursive check
	// if tab state, set current tab index
	cDockWindow *tabWnd = dock->m_tabs.empty() ? dock->m_parent : dock;
	RETV(tabWnd->m_tabs.empty(), nullptr);

	++tabWnd->m_selectTab;
	tabWnd->m_selectTab %= (int)(tabWnd->m_tabs.size()+1);

	if (tabWnd->m_selectTab == 0)
		return tabWnd;
	return tabWnd->m_tabs[tabWnd->m_selectTab - 1];
}


void cRenderWindow::SetCapture(cDockWindow *dock)
{
	m_captureDock = dock;
	::SetCapture(getSystemHandle());
}


void cRenderWindow::SetFocus(cDockWindow *dock)
{
	m_focusDock = dock;
}


cDockWindow* cRenderWindow::GetCapture()
{
	return m_captureDock;
}


cDockWindow* cRenderWindow::GetFocus()
{
	return m_focusDock;
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
	case eState::NORMAL_DOWN_ETC: return "Normal_Etc";
	case eState::WINDOW_RESIZE: return "Window_Resize";
	case eState::DOCK_SIZE: return "Resize";
	case eState::DRAG: return "Drag";
	case eState::MOVE: return "Move";
	case eState::DRAG_BIND: return "Drag_Bind";
	default: assert(0); return "None";
	}
}
