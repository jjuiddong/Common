#pragma once
// ImGui Win32 + DirectX9 binding
// In this binding, ImTextureID is used to store a 'LPDIRECT3DTEXTURE9' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

struct IDirect3DDevice9;

//IMGUI_API bool        ImGui_ImplDX9_Init(void* hwnd, IDirect3DDevice9* device);
//IMGUI_API void        ImGui_ImplDX9_Shutdown();
//IMGUI_API void        ImGui_ImplDX9_NewFrame();
//
//// Use if you want to reset your rendering device without losing ImGui state.
//IMGUI_API void        ImGui_ImplDX9_InvalidateDeviceObjects();
//IMGUI_API bool        ImGui_ImplDX9_CreateDeviceObjects();

// Handler for Win32 messages, update mouse/keyboard data.
// You may or not need this for your implementation, but it can serve as reference for handling inputs.
// Commented out to avoid dragging dependencies on <windows.h> types. You can copy the extern declaration in your code.
/*
IMGUI_API LRESULT   ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
*/


// customize, jjuiddong
// DirectX 9
class cImGui
{
public:
	cImGui();
	virtual ~cImGui();

	bool Init(void* hwnd, IDirect3DDevice9* device);
	LRESULT WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam);
	void Shutdown();
	void NewFrame();
	void InvalidateDeviceObjects();
	bool CreateDeviceObjects();
	void Render();
	void RenderDrawLists(ImDrawData* draw_data);
	bool CreateFontsTexture();
	void SetContext();
	void SetNullContext();
	

public:
	HWND g_hWnd = 0;
	INT64 g_Time = 0;
	INT64 g_TicksPerSecond = 0;
	LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
	LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
	LPDIRECT3DINDEXBUFFER9 g_pIB = NULL;
	LPDIRECT3DTEXTURE9 g_FontTexture = NULL;
	int g_VertexBufferSize = 5000;
	int g_IndexBufferSize = 10000;
	ImGuiContext *m_context;
	ImFontAtlas m_FontAtlas;
};
