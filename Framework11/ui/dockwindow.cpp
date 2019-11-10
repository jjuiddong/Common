
#include "stdafx.h"
#include "dockwindow.h"
#include "renderwindow.h"
#include "dockmanager.h"
#include "../external/imgui/imgui_internal.h"


using namespace graphic;
using namespace framework;


cDockWindow::cDockWindow(const StrId &name //=""
)
	: m_state(eDockState::DOCKWINDOW)
	, m_name(name)
	, m_isDockBinding(false)
	, m_dockSlot(eDockSlot::TOP)
	, m_owner(NULL)
	, m_lower(NULL)
	, m_upper(NULL)
	, m_parent(NULL)
	, m_rect(0, 0, 0, 0)
	, m_selectTab(0)
	, m_dragSlot(eDockSlot::NONE)
	, m_camera("Dock Camera")
	, m_sizingOpt(eDockSizingOption::RATE)
{
}

cDockWindow::~cDockWindow()
{
	Clear();
}


bool cDockWindow::Create(const eDockState::Enum state, const eDockSlot::Enum type,
	cRenderWindow *owner, cDockWindow *parent
	, const float windowSize//=0.5f
	, const eDockSizingOption::Enum option //= eDockSizingOption::RATE
)
{
	m_state = state;
	m_dockSlot = type;
	m_owner = owner;
	m_parent = parent;
	m_sizingOpt = option;

	if (owner)
	{
		if (!owner->m_dock)
		{
			owner->m_dock = this;

			const sf::Vector2u size = m_owner->getSize();
			m_rect = sRectf(0, m_owner->m_isTitleBar? 0 : TITLEBAR_HEIGHT2, (float)size.x, (float)size.y);
		}
	}

	if (parent)
	{
		if (state == eDockState::DOCKWINDOW)
			parent->Dock(type, this, windowSize);
	}

	return true;
}


bool cDockWindow::Dock(const eDockSlot::Enum type
	, cDockWindow *child
	, const float windowSize //= 0.5f
)
{
	if (this == child)
		return false;

	if (eDockSlot::TAB == type)
	{
		child->m_owner = m_owner;
		child->m_parent = this;
		child->m_dockSlot = m_dockSlot;
		child->m_rect = m_rect;
		m_tabs.push_back(child);

		ResizeEnd(eDockResize::DOCK_WINDOW, m_rect);
		return true;
	}

	cDockWindow *newVirtualDock = cDockManager::Get()->NewDockWindow();
	newVirtualDock->m_state = eDockState::VIRTUAL;
	newVirtualDock->m_dockSlot = m_dockSlot;
	newVirtualDock->m_rect = m_rect;
	newVirtualDock->m_owner = m_owner;

	m_dockSlot = eDockSlot::GetOpposite(type);
	switch (type)
	{
	case eDockSlot::LEFT:
	case eDockSlot::TOP:
		newVirtualDock->m_lower = child;
		newVirtualDock->m_upper = this;
		break;

	case eDockSlot::RIGHT:
	case eDockSlot::BOTTOM:
		newVirtualDock->m_lower = this;
		newVirtualDock->m_upper = child;
		break;

	default: assert(0);
	}

	ReplaceDockWindow(newVirtualDock);

	m_parent = newVirtualDock;
	child->m_owner = m_owner;
	child->m_parent = newVirtualDock;
	child->m_dockSlot = type;
	InitializeWindowSize(child, windowSize);

	return true;
}


bool cDockWindow::Undock(const bool newWindow // = true
)
{
	if (m_parent)
	{
		m_parent->Undock(this);
	}
	else
	{
		if (cDockWindow *showWnd = UndockTab())
		{
			ReplaceDockWindow(showWnd);
		}
		else
		{
			ReplaceDockWindow(NULL);
			cDockManager::Get()->DeleteRenderWindow(m_owner);
		}
	}

	if (newWindow)
	{
		ReleaseCapture();

		const float width = 800;
		const float height = 600;
		cImGui *oldGui = &m_owner->m_gui;
		cRenderWindow *window = cDockManager::Get()->NewRenderWindow(m_name.c_str(), width, height);
		window->m_dock = this;
		m_parent = NULL;
		SendMessage(m_owner->getSystemHandle(), WM_LBUTTONUP, 0, 0); // bug fix (ImGui::ItemActive() bug)
		m_owner = window;
		CalcResizeWindow(eDockResize::RENDER_WINDOW, sRectf(0, 0, width, height));
		oldGui->SetContext();
		window->requestFocus();
		window->SetDragState();
		window->RequestResetDeviceNextFrame();
		SetCapture();
	}
	else
	{
		m_parent = NULL;
		m_owner = NULL;
	}

	return true;
}


// Undock in Child Window dock ptr
bool cDockWindow::Undock(cDockWindow *udock)
{
	RETV(this == udock, false);

	cDockWindow *rmWnd = NULL;
	cDockWindow *showWnd = NULL;

	if (m_lower == udock)
	{
		rmWnd = udock;
		showWnd = m_upper;
	}
	else if (m_upper == udock)
	{
		rmWnd = udock;
		showWnd = m_lower;
	}
	else
	{
		if (RemoveTab(udock))
			return true;

		assert(0);
		return false;
	}

	if (rmWnd->m_tabs.empty())
	{
		ReplaceDockWindow(showWnd);
		udock->Merge(showWnd);
		showWnd->m_dockSlot = m_dockSlot;
		showWnd->ResizeEnd(eDockResize::DOCK_WINDOW, showWnd->m_rect);
		cDockManager::Get()->DeleteDockWindow(this);
	}
	else
	{
		showWnd = rmWnd->UndockTab();

		if (m_lower == udock)
		{
			m_lower = showWnd;
		}
		else if (m_upper == udock)
		{
			m_upper = showWnd;
		}
		else
		{
			assert(0);
		}
	}

	return true;
}


// remove this DockWindow from Tab
// return Show DockWindow
cDockWindow* cDockWindow::UndockTab()
{
	RETV(m_tabs.empty(), NULL);

	cDockWindow *showWnd = m_tabs[0];
	common::popvector(m_tabs, 0);

	showWnd->m_parent = m_parent;
	showWnd->m_dockSlot = m_dockSlot;
	showWnd->m_lower = m_lower;
	showWnd->m_upper = m_upper;
	showWnd->m_tabs = m_tabs;
	for (auto p : showWnd->m_tabs)
		p->m_parent = showWnd;	
	m_tabs.clear();

	return showWnd;
}


// Merge Current Space and Dock Space
// and then, Remove Current Dock Window
bool cDockWindow::Merge(cDockWindow *dock)
{
	sRectf rect;
	if (((m_dockSlot == eDockSlot::LEFT) && (dock->m_dockSlot == eDockSlot::RIGHT))
		|| ((m_dockSlot == eDockSlot::RIGHT) && (dock->m_dockSlot == eDockSlot::LEFT)))
	{
		rect = sRectf(min(m_rect.left, dock->m_rect.left)
			, m_rect.top
			, max(m_rect.right, dock->m_rect.right)
			, m_rect.bottom);
	}
	else if (((m_dockSlot == eDockSlot::TOP) && (dock->m_dockSlot == eDockSlot::BOTTOM))
		|| ((m_dockSlot == eDockSlot::BOTTOM) && (dock->m_dockSlot == eDockSlot::TOP)))
	{
		rect = sRectf(m_rect.left
			, min(m_rect.top, dock->m_rect.top)
			, m_rect.right
			, max(m_rect.bottom, dock->m_rect.bottom));
	}
	else
	{
		assert(0);
		return false;
	}

	dock->CalcResizeWindow(eDockResize::DOCK_WINDOW, rect);
	return true;
}


// Replace DockWindow from argument dock Ptr
void cDockWindow::ReplaceDockWindow(cDockWindow *dock)
{
	if (m_parent)
	{
		if (m_parent->m_lower == this)
			m_parent->m_lower = dock;
		else if (m_parent->m_upper == this)
			m_parent->m_upper = dock;
		else
			assert(0);

		if (dock)
			dock->m_parent = m_parent;
	}
	else if (m_owner)
	{
		if (m_owner->m_dock == this)
			m_owner->m_dock = dock;
		else
			assert(0);

		if (dock)
			dock->m_parent = NULL;
	}
	else
	{
		assert(0);
	}
}


using namespace ImGui;
ImRect getSlotRect(const ImRect &parent_rect, eDockSlot::Enum dock_slot)
{
	ImVec2 size = parent_rect.Max - parent_rect.Min;
	ImVec2 center = parent_rect.Min + size * 0.5f;
	switch (dock_slot)
	{
	default: return ImRect(center - ImVec2(20, 20), center + ImVec2(20, 20));
	case eDockSlot::TOP: return ImRect(center + ImVec2(-20, -50), center + ImVec2(20, -30));
	case eDockSlot::RIGHT: return ImRect(center + ImVec2(30, -20), center + ImVec2(50, 20));
	case eDockSlot::BOTTOM: return ImRect(center + ImVec2(-20, +30), center + ImVec2(20, 50));
	case eDockSlot::LEFT: return ImRect(center + ImVec2(-50, -20), center + ImVec2(-30, 20));
	}
}


eDockSlot::Enum cDockWindow::render_dock_slot_preview(const ImVec2& mouse_pos, const ImVec2& cPos, const ImVec2& cSize)
{
	eDockSlot::Enum dock_slot = eDockSlot::NONE;

	ImRect rect{ cPos, cPos + cSize };

	auto checkSlot = [&mouse_pos](ImRect rect, eDockSlot::Enum slot) -> bool
	{
		auto slotRect = getSlotRect(rect, slot);
		ImGui::GetWindowDrawList()->AddRectFilled(
			slotRect.Min,
			slotRect.Max,
			ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.8f)));//tab
		return slotRect.Contains(mouse_pos);
	};

	if (checkSlot(rect, eDockSlot::TAB))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockSlot::TAB;
	}

	if (checkSlot(rect, eDockSlot::LEFT))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockSlot::LEFT;
	}

	if (checkSlot(rect, eDockSlot::RIGHT))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y), ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockSlot::RIGHT;
	}

	if (checkSlot(rect, eDockSlot::TOP))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + cSize.x, cPos.y + (cSize.y / 2.0f)), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockSlot::TOP;
	}

	if (checkSlot(rect, eDockSlot::BOTTOM))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cPos.x, cPos.y + (cSize.y / 2.0f)), ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockSlot::BOTTOM;
	}

	return dock_slot;
}


void cDockWindow::RenderDock( const float deltaSeconds
	, const Vector2 &pos //=ImVec2(0,0)	
	) 
{
	if (m_lower)
	{
		m_lower->RenderDock(deltaSeconds);

		if (m_upper)
			m_upper->RenderDock(deltaSeconds, pos);
	}
	else
	{
		Vector2 npos = pos + Vector2(m_rect.left, m_rect.top);
		ImGui::SetCursorPos(ImVec2(npos.x, npos.y));
		RenderTab();
		npos.y += TAB_H;
		ImGui::SetCursorPos(ImVec2(npos.x, npos.y));

		ImGui::BeginChild(m_name.c_str(),
			ImVec2((float)m_rect.Width()-3, (float)m_rect.Height() - (TAB_H+3)), false,
			ImGuiWindowFlags_AlwaysUseWindowPadding);

		if (m_selectTab == 0)
			OnRender(deltaSeconds);
		else if ((int)m_tabs.size() > m_selectTab-1)
			m_tabs[m_selectTab-1]->OnRender(deltaSeconds);

		if (m_isDockBinding)
		{
			const ImVec2 size(m_rect.Width(), m_rect.Height());
			const ImVec2 screen_cursor_pos = ImVec2(npos.x, npos.y - TAB_H);
			const auto mouse_pos = sf::Mouse::getPosition(*m_owner);
			const ImVec2 cursor_pos = { float(mouse_pos.x), float(mouse_pos.y) };
			if ((mouse_pos.x > screen_cursor_pos.x && mouse_pos.x < (screen_cursor_pos.x + size.x)) &&
				(mouse_pos.y > screen_cursor_pos.y && mouse_pos.y < (screen_cursor_pos.y + size.y)))
			{
				// todo: cliprect bugfix, only scroll work cliprect
				ImGui::SetScrollY(ImGui::GetScrollY() + ImGui::GetWindowSize().y);

				ImGui::BeginChild("##dockSlotPreview");
				ImGui::PushClipRect(ImVec2(), ImGui::GetIO().DisplaySize, false);
				m_dragSlot = render_dock_slot_preview(cursor_pos, screen_cursor_pos, ImVec2(m_rect.Width(), m_rect.Height()));
				ImGui::PopClipRect();
				ImGui::EndChild();

				if (m_dragSlot != eDockSlot::NONE)
				{
					cDockManager::Get()->SetDragTarget(this);
				}
			}
		}

		ImGui::EndChild();
	}
}


void cDockWindow::RenderTab()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 3.0f));
	ImVec4 childBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];

	for (u_int i = 0; i < m_tabs.size()+1; ++i)
	{
		if (i == m_selectTab)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, childBg);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, childBg);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, childBg);
		}
		else
		{
			ImVec4 button_color = ImVec4(childBg.x - 0.04f, childBg.y - 0.04f, childBg.z - 0.04f, childBg.w);
			ImVec4 buttonColorActive = ImVec4(childBg.x + 0.10f, childBg.y + 0.10f, childBg.z + 0.10f, childBg.w);
			ImVec4 buttonColorHovered = ImVec4(childBg.x + 0.15f, childBg.y + 0.15f, childBg.z + 0.15f, childBg.w);
			ImGui::PushStyleColor(ImGuiCol_Button, button_color);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColorActive);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColorHovered);
		}

		cDockWindow *dock = (i == 0) ? this : m_tabs[i-1];
		if (ImGui::Button(dock->m_name.c_str(), ImVec2(0, TAB_H)))
		{
			m_selectTab = i;
		}

		if (ImGui::IsItemActive()  
			&& (GetAsyncKeyState(VK_LBUTTON) & 0x8000) // drag bug fix
													// flicking docking windowMouse Left Button Down
			&& !cDockManager::Get()->IsDragState() 
			&& !cDockManager::Get()->IsMoveState())
		{
			const bool IsMainWindow = cDockManager::Get()->m_mainWindow == m_owner;
			if (!IsMainWindow)
				m_owner->SetTabClickState();

			const float delta = m_owner->m_clickPos.y - ImGui::GetIO().MousePos.y;
			if (!m_parent && !m_upper && !m_lower && m_tabs.empty())
			{ // SingleTab DockWindow
				if (delta < -2 || delta > 2)
				{
					if (!IsMainWindow)
						m_owner->SetDragState();
				}
			}
			else if (delta < -4 || delta > 27)
			{ // MultiTab DockWindow
				if (i == 0)
				{
					Undock();
				}
				else
				{
					m_tabs[i - 1]->Undock();
				}
			}
		}

		ImGui::SameLine();
		ImGui::PopStyleColor(3);
	}

	ImGui::PopStyleVar();
}


void cDockWindow::PreRender(const float deltaSeconds)
{
	if (m_lower)
	{
		m_lower->PreRender(deltaSeconds);
	}
	else
	{
		if (m_selectTab == 0)
			OnPreRender(deltaSeconds);
		else if ((int)m_tabs.size() > m_selectTab - 1)
			m_tabs[m_selectTab - 1]->PreRender(deltaSeconds);
	}

	if (m_upper)
	{
		m_upper->PreRender(deltaSeconds);
	}
}


void cDockWindow::PostRender(const float deltaSeconds)
{
	if (m_lower)
	{
		m_lower->PostRender(deltaSeconds);
	}
	else
	{
		if (m_selectTab == 0)
			OnPostRender(deltaSeconds);
		else if ((int)m_tabs.size() > m_selectTab - 1)
			m_tabs[m_selectTab - 1]->PostRender(deltaSeconds);
	}

	if (m_upper)
	{
		m_upper->PostRender(deltaSeconds);
	}
}


void cDockWindow::Update(const float deltaSeconds)
{
	OnUpdate(deltaSeconds);

	if (m_lower)
		m_lower->Update(deltaSeconds);
	for (auto &p : m_tabs)
		p->Update(deltaSeconds);
	if (m_upper)
		m_upper->Update(deltaSeconds);
}


void cDockWindow::InitializeWindowSize(cDockWindow *dock
	, const float windowSize //= 0.5f
)
{
	RET(!dock);

	const sf::Vector2f size(m_rect.Width(), m_rect.Height());
	const float x = m_rect.left;
	const float y = m_rect.top;
	sRectf rect1, rect2;

	switch (dock->m_dockSlot)
	{
	case eDockSlot::TAB:
		rect1 = sRectf(x, y, x + size.x, y + size.y);
		rect2 = sRectf(x, y, x + size.x, y + size.y);
		break;

	case eDockSlot::LEFT:
		rect1 = sRectf(x + size.x *windowSize, y, x+size.x, y+size.y);
		rect2 = sRectf(x, y, x+size.x*windowSize, y+size.y);
		break;

	case eDockSlot::RIGHT:
		rect1 = sRectf(x, y, x + size.x *(1-windowSize), y + size.y);
		rect2 = sRectf(x + size.x *(1-windowSize), y, x + size.x, y + size.y);
		break;

	case eDockSlot::TOP:
		rect1 = sRectf(x, y + size.y *windowSize, x + size.x, y + size.y);
		rect2 = sRectf(x, y, x + size.x, y + size.y *windowSize);
		break;

	case eDockSlot::BOTTOM:
		rect1 = sRectf(x, y, x + size.x, y + size.y *(1-windowSize));
		rect2 = sRectf(x, y + size.y *(1-windowSize), x + size.x, y + size.y);
		break;

	case eDockSlot::LEFT_MOST:
	case eDockSlot::TOP_MOST:
	case eDockSlot::RIGHT_MOST:
	case eDockSlot::BOTTOM_MOST:
		break;
	}

	CalcResizeWindow(eDockResize::DOCK_WINDOW, rect1);
	dock->CalcResizeWindow(eDockResize::DOCK_WINDOW, rect2);

	OnResizeEnd(eDockResize::DOCK_WINDOW, rect1);
	dock->OnResizeEnd(eDockResize::DOCK_WINDOW, rect2);
}


// Update Window size, from already setting width/height rate
// Only Change Lower Window, Upper Window Is Fixed
void cDockWindow::CalcResizeWindow(const eDockResize::Enum type, const sRectf &rect)
{
	RET(m_rect == rect);

	const float w = m_rect.Width();
	const float h = m_rect.Height();
	const float nw = rect.Width();
	const float nh = rect.Height();

	if (m_upper && m_lower)
	{
		const eDockSizingOption::Enum lowerOpt = m_lower->GetSizingOption();
		const eDockSizingOption::Enum upperOpt = m_upper->GetSizingOption();

		if ((lowerOpt == eDockSizingOption::PIXEL) 
			|| (upperOpt == eDockSizingOption::PIXEL))
		{
			float pixel = 0;
			switch (m_lower->m_dockSlot)
			{
			case eDockSlot::LEFT: 
				pixel = (lowerOpt == eDockSizingOption::PIXEL) ? m_lower->m_rect.Width() : (nw - m_upper->m_rect.Width()); 
				pixel = min(max(30.f, pixel), nw-30.f);
				break;
			case eDockSlot::TOP: 
				pixel = (lowerOpt == eDockSizingOption::PIXEL) ? m_lower->m_rect.Height() : (nh - m_upper->m_rect.Height());
				pixel = min(max(30.f, pixel), nh-30.f);
				break;
			default: assert(0); break;
			}

			switch (m_lower->m_dockSlot)
			{
			case eDockSlot::LEFT:
				m_lower->CalcResizeWindow(type, sRectf(rect.left, rect.top, rect.left + pixel, rect.bottom));
				m_upper->CalcResizeWindow(type, sRectf(rect.left+pixel, rect.top, rect.right, rect.bottom));
				break;

			case eDockSlot::TOP:
				m_lower->CalcResizeWindow(type, sRectf(rect.left, rect.top, rect.right, rect.top + pixel));
				m_upper->CalcResizeWindow(type, sRectf(rect.left, rect.top+pixel, rect.right, rect.bottom));
				break;
			default: assert(0); break;
			}
		}
		else
		{ // All RATE Option
			Vector4 v0(rect.left, rect.top, rect.left, rect.top);
			Vector4 v1(m_rect.left, m_rect.top, m_rect.left, m_rect.top);

			// lower
			{
				cDockWindow *p = m_lower;
				Vector4 v2(p->m_rect.left, p->m_rect.top, p->m_rect.right, p->m_rect.bottom);
				Vector4 v3 = v2 - v1;
				v3.x = (v3.x / w) * nw; // left
				v3.y = (v3.y / h) * nh; // top
				v3.z = (v3.z / w) * nw; // right
				v3.w = (v3.w / h) * nh; // bottom
				Vector4 v4 = v3 + v0;
				sRectf nr(v4.x, v4.y, v4.z, v4.w);
				m_lower->CalcResizeWindow(type, nr);
			}

			// upper
			{
				cDockWindow *p = m_upper;
				Vector4 v2(p->m_rect.left, p->m_rect.top, p->m_rect.right, p->m_rect.bottom);
				Vector4 v3 = v2 - v1;
				v3.x = (v3.x / w) * nw; // left
				v3.y = (v3.y / h) * nh; // top
				v3.z = (v3.z / w) * nw; // right
				v3.w = (v3.w / h) * nh; // bottom
				Vector4 v4 = v3 + v0;
				sRectf nr(v4.x, v4.y, v4.z, v4.w);
				m_upper->CalcResizeWindow(type, nr);
			}
		}
	}
	
	m_rect = rect;

	for (auto &p : m_tabs)
		p->CalcResizeWindow(type, rect);

	OnResize(type, rect);
}


void cDockWindow::CalcResizeWindow(const eDockResize::Enum type, const int deltaSize)
{
	RET(m_state != eDockState::VIRTUAL);
	RET(!m_lower || !m_upper);

	const float minCx = 50;
	const float minCy = 100;

	sRectf rect1 = m_lower->m_rect;
	sRectf rect2 = m_upper->m_rect;
	const eDockSlot::Enum dockType = m_lower->m_dockSlot;
	switch (dockType)
	{
	case eDockSlot::LEFT:
		rect1.right += (float)deltaSize;
		rect2.left += (float)deltaSize;

		rect1.right = max(m_rect.left + minCx, rect1.right);
		rect1.right = min(m_rect.right - minCx, rect1.right);
		rect2.left = max(m_rect.left + minCx, rect2.left);
		rect2.left = min(m_rect.right - minCx, rect2.left);
		break;

	case eDockSlot::TOP:
		rect1.bottom += (float)deltaSize;
		rect2.top += (float)deltaSize;

		rect1.bottom = max(m_rect.top + minCy, rect1.bottom);
		rect1.bottom = min(m_rect.bottom - minCy, rect1.bottom);
		rect2.top = max(m_rect.top + minCy, rect2.top);
		rect2.top = min(m_rect.bottom - minCy, rect2.top);
		break;

	case eDockSlot::RIGHT:
	case eDockSlot::BOTTOM:
		assert(0); // never reach this code
		break;

	case eDockSlot::LEFT_MOST:
	case eDockSlot::RIGHT_MOST:
	case eDockSlot::TOP_MOST:
	case eDockSlot::BOTTOM_MOST:
	default: assert(0);
	}
	
	m_lower->CalcResizeWindow(type, rect1);
	m_upper->CalcResizeWindow(type, rect2);
}


// 도킹화면에서 탭버튼을 제외한 실제 사용할 수 있는 윈도우화면 크기를 리턴한다.
sRectf cDockWindow::GetWindowSizeAvailible(
	const bool ignoreRightSide //= false
)
{
	sRectf r;
	r.left = 9.f; // side border
	r.top = (TAB_H + 3.f); // tab button
	r.right = m_rect.Width() - 9.f; // side border
	r.bottom = m_rect.Height() - 20.f; // scrollbar bug fix

	// 윈도우 화면의 오른쪽 부분을 일정크기만큼 제외한다.
	// 도킹윈도우에서 화면크기를 제어할 때, 겹치는 것을 막기위해서다.
	if (ignoreRightSide)
		r.right -= 10.f;

	return r;
}


// 탭에서 빠진 윈도우는 외부에서 메모리를 제거한다.
bool cDockWindow::RemoveTab(cDockWindow *tab)
{
	RETV(!tab, false);
	RETV(m_tabs.empty(), false);

	if (!common::removevector(m_tabs, tab))
		return false;

	m_selectTab = 0;
	return true;
}


void cDockWindow::ResizeEnd(const eDockResize::Enum type, const sRectf &rect)
{
	OnResizeEnd(type, rect);

	if (m_lower)
		m_lower->ResizeEnd(type, rect);
	for (auto &p : m_tabs)
		p->ResizeEnd(type, rect);
	if (m_upper)
		m_upper->ResizeEnd(type, rect);
}


bool cDockWindow::IsInSizerSpace(const Vector2 &pos)
{
	RETV(m_state != eDockState::VIRTUAL, false);

	const float SPACE = 4; //=10

	sRectf rect;
	const eDockSlot::Enum dockType = m_lower->m_dockSlot;
	switch (dockType)
	{
	case eDockSlot::LEFT:
		rect = sRectf(m_lower->m_rect.right - SPACE
			, m_rect.top
			, m_lower->m_rect.right + SPACE
			, m_rect.bottom
		);
		break;

	case eDockSlot::TOP:
		rect = sRectf(m_rect.left
			, m_lower->m_rect.bottom - SPACE
			, m_rect.right
			, m_lower->m_rect.bottom + SPACE
		);
		break;

	default: assert(0);
	}

	return rect.IsIn(pos.x, pos.y);
}

// pos: dockwindow local mouse pos
bool cDockWindow::IsInWindow(const Vector2 &pos)
{
	// dockwindow -> renderwindow coordinate system
	Vector2 tmp = pos;
	tmp.x += m_rect.left;
	tmp.y += m_rect.top;
	return m_rect.IsIn(tmp.x, tmp.y);
}

// pos: dockwindow local mouse pos
bool cDockWindow::IsInWindow(const POINT &pos)
{
	return IsInWindow(Vector2((float)pos.x, (float)pos.y));
}


eDockSizingType::Enum cDockWindow::GetDockSizingType()
{
	const eDockSlot::Enum dockType = m_lower->m_dockSlot;
	switch (dockType)
	{
	case eDockSlot::LEFT: return eDockSizingType::HORIZONTAL;
	case eDockSlot::TOP: return eDockSizingType::VERTICAL;
	default: break;
	}
	return eDockSizingType::NONE;
}


void cDockWindow::SetBindState(const bool enable) // enable = true;
{
	m_isDockBinding = enable;
	if (m_lower)
		m_lower->SetBindState(enable);
	if (m_upper)
		m_upper->SetBindState(enable);
}


void cDockWindow::LostDevice()
{
	OnLostDevice();

	if (m_lower)
		m_lower->LostDevice();
	for (auto &p : m_tabs)
		p->LostDevice();
	if (m_upper)
		m_upper->LostDevice();
}


void cDockWindow::ResetDevice()
{
	OnResetDevice();

	if (m_lower)
		m_lower->ResetDevice();
	for (auto &p : m_tabs)
		p->ResetDevice();
	if (m_upper)
		m_upper->ResetDevice();
}


void cDockWindow::ClearConnection()
{
	m_lower = NULL;
	m_upper = NULL;
	m_parent = NULL;
	m_owner = NULL;
	m_tabs.clear();
}


void cDockWindow::DefaultEventProc(const sf::Event &evt)
{
	if (!CheckEventTarget(evt))
		return;

	if (m_lower)
	{
		m_lower->DefaultEventProc(evt);
	}
	else
	{
		if (m_selectTab == 0)
			OnEventProc(evt);
		else if ((int)m_tabs.size() > m_selectTab - 1)
			m_tabs[m_selectTab - 1]->DefaultEventProc(evt);
	}

	if (m_upper)
	{
		m_upper->DefaultEventProc(evt);
	}
}


// Picking Event Trigger
void cDockWindow::TriggerPickingEvent(const POINT &mousePos)
{
	ePickState::Enum state = ePickState::HOVER;
	if (GetInput().m_mouseClicked[0])
		state = ePickState::CLICK;
	else if (GetInput().m_lBtnDbClick)
		state = ePickState::DBCLICK;

	m_pickMgr.Pick(0, mousePos, state);
}


bool cDockWindow::CheckEventTarget(const sf::Event &evt)
{
	if (GetCapture() == this)
		return true;

	switch (evt.type)
	{
	case sf::Event::MouseButtonPressed:
	{
		POINT pos = { evt.mouseButton.x, evt.mouseButton.y };
		if (!m_rect.IsIn((float)pos.x, (float)pos.y))
			return false;
	}
	break;

	case sf::Event::MouseWheelScrolled:
	{
		POINT pos = { evt.mouseWheelScroll.x, evt.mouseWheelScroll.y };
		if (!m_rect.IsIn((float)pos.x, (float)pos.y))
			return false;
	}
	break;

	case sf::Event::MouseWheelMoved:
	{
		POINT pos = { evt.mouseWheel.x, evt.mouseWheel.y };
		if (!m_rect.IsIn((float)pos.x, (float)pos.y))
			return false;
	}
	break;

	case sf::Event::MouseMoved:
	{
		POINT pos = { evt.mouseMove.x, evt.mouseMove.y };
		if (!m_rect.IsIn((float)pos.x, (float)pos.y))
			return false;
	}
	break;

	case sf::Event::MouseButtonReleased:
		break;
	}

	return true;
}


void cDockWindow::SetCapture()
{
	if (m_owner)
		m_owner->SetCapture(this);
}


cDockWindow* cDockWindow::GetCapture()
{
	if (m_owner)
		return m_owner->GetCapture();
	return NULL;
}


void cDockWindow::ReleaseCapture()
{
	if (m_owner)
		m_owner->ReleaseCapture();
}


cRenderer& cDockWindow::GetRenderer()
{
	return m_owner->m_renderer;
}


framework::cInputManager& cDockWindow::GetInput()
{
	return m_owner->m_input;
}


eDockSizingOption::Enum cDockWindow::GetSizingOption()
{
	if (eDockState::DOCKWINDOW == m_state)
		return m_sizingOpt;
	return m_lower->GetSizingOption();
}


// Delete All Connection Docking Window
void cDockWindow::Clear()
{
	for (auto &p : m_tabs)
		SAFE_DELETE(p);
	m_tabs.clear();

	SAFE_DELETE(m_lower);
	SAFE_DELETE(m_upper);
}
