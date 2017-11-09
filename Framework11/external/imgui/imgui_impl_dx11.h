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

	bool Init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context, ImFontAtlas *font);
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
	INT64                    m_Time = 0;
	INT64                    m_TicksPerSecond = 0;

	HWND                     m_hWnd = 0;
	ID3D11Device*            m_pd3dDevice = NULL;
	ID3D11DeviceContext*     m_pd3dDeviceContext = NULL;
	ID3D11Buffer*            m_pVB = NULL;
	ID3D11Buffer*            m_pIB = NULL;
	ID3D10Blob *             m_pVertexShaderBlob = NULL;
	ID3D11VertexShader*      m_pVertexShader = NULL;
	ID3D11InputLayout*       m_pInputLayout = NULL;
	ID3D11Buffer*            m_pVertexConstantBuffer = NULL;
	ID3D10Blob *             m_pPixelShaderBlob = NULL;
	ID3D11PixelShader*       m_pPixelShader = NULL;
	ID3D11SamplerState*      m_pFontSampler = NULL;
	ID3D11ShaderResourceView*m_pFontTextureView = NULL;
	ID3D11RasterizerState*   m_pRasterizerState = NULL;
	ID3D11BlendState*        m_pBlendState = NULL;
	ID3D11DepthStencilState* m_pDepthStencilState = NULL;
	int                      m_VertexBufferSize = 5000, m_IndexBufferSize = 10000;

	ImGuiContext *m_context;
	ImFontAtlas *m_FontAtlas;
};
