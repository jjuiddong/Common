#pragma once
// ImGui Win32 + DirectX11 binding
// In this binding, ImTextureID is used to store a 'ID3D11ShaderResourceView*' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

struct ID3D11Device;
struct ID3D11DeviceContext;

//IMGUI_API bool        ImGui_ImplDX11_Init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context);
//IMGUI_API void        ImGui_ImplDX11_Shutdown();
//IMGUI_API void        ImGui_ImplDX11_NewFrame();
//
//// Use if you want to reset your rendering device without losing ImGui state.
//IMGUI_API void        ImGui_ImplDX11_InvalidateDeviceObjects();
//IMGUI_API bool        ImGui_ImplDX11_CreateDeviceObjects();

// Handler for Win32 messages, update mouse/keyboard data.
// You may or not need this for your implementation, but it can serve as reference for handling inputs.
// Commented out to avoid dragging dependencies on <windows.h> types. You can copy the extern declaration in your code.
/*
IMGUI_API LRESULT   ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
*/


// customize, jjuiddong
// DirectX 11
class cImGui
{
public:
	cImGui();
	virtual ~cImGui();

	bool Init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context);
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
	//HWND g_hWnd = 0;
	//INT64 g_Time = 0;
	//INT64 g_TicksPerSecond = 0;
	//LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
	//LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
	//LPDIRECT3DINDEXBUFFER9 g_pIB = NULL;
	//LPDIRECT3DTEXTURE9 g_FontTexture = NULL;
	//int g_VertexBufferSize = 5000;
	//int g_IndexBufferSize = 10000;
	//ImGuiContext *m_context;
	//ImFontAtlas m_FontAtlas;

	INT64                    g_Time = 0;
	INT64                    g_TicksPerSecond = 0;

	HWND                     g_hWnd = 0;
	ID3D11Device*            g_pd3dDevice = NULL;
	ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
	ID3D11Buffer*            g_pVB = NULL;
	ID3D11Buffer*            g_pIB = NULL;
	ID3D10Blob *             g_pVertexShaderBlob = NULL;
	ID3D11VertexShader*      g_pVertexShader = NULL;
	ID3D11InputLayout*       g_pInputLayout = NULL;
	ID3D11Buffer*            g_pVertexConstantBuffer = NULL;
	ID3D10Blob *             g_pPixelShaderBlob = NULL;
	ID3D11PixelShader*       g_pPixelShader = NULL;
	ID3D11SamplerState*      g_pFontSampler = NULL;
	ID3D11ShaderResourceView*g_pFontTextureView = NULL;
	ID3D11RasterizerState*   g_pRasterizerState = NULL;
	ID3D11BlendState*        g_pBlendState = NULL;
	ID3D11DepthStencilState* g_pDepthStencilState = NULL;
	int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

	ImGuiContext *m_context;
	ImFontAtlas m_FontAtlas;
};
