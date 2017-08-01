//
// jjuiddong
// Renderer
//
#pragma once


namespace graphic
{

	class cRenderer
	{
	public:
		cRenderer();
		virtual ~cRenderer();

		bool CreateDirectX(HWND hWnd, const int width, const int height
			, const UINT adapter = D3DADAPTER_DEFAULT
			);
		void Update(const float elpaseT);
		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDevContext();
		HWND GetHwnd() const;
		bool CheckResetDevice(const int width=0, const int height=0);
		bool ResetDevice(const int width=0, const int height=0, const bool forceReset=false
			, const bool resetResource=true);
		void AddRenderAlpha(cNode2 *node, const Vector3 &normal = Vector3(0, 0, 1), const Matrix44 &tm=Matrix44::Identity, const int opt= 1);
		void AddRenderAlpha(sAlphaBlendSpace *space, cNode2 *node, const Vector3 &normal = Vector3(0, 0, 1), const Matrix44 &tm = Matrix44::Identity, const int opt = 1);
		void AddAlphaBlendSpace(const cBoundingBox &bbox);
		sAlphaBlendSpace* GetCurrentAlphaBlendSpace();


		bool ClearScene();
		void BeginScene();
		void Present();
		void EndScene();

		void RenderAxis();
		void RenderFPS();
		void RenderGrid();

		void SetLightEnable(const int light, const bool enable);

		void SetCullMode(const D3DCULL cull);
		void SetFillMode(const D3DFILLMODE mode);
		void SetNormalizeNormals(const bool value);
		void SetAlphaBlend(const bool value);
		void SetZEnable(const bool value);
		void SetZFunc(const D3DCMPFUNC value);

		D3DFILLMODE GetFillMode();
		D3DCULL GetCullMode();


	protected:
		void MakeGrid( const float width, const int count, DWORD color, vector<sVertexDiffuse> &out );
		void MakeAxis( const float length, DWORD xcolor, DWORD ycolor, DWORD zcolor, vector<sVertexDiffuse> &out );


	public:
		HWND m_hWnd;
		ID3D11Device *m_d3dDevice;
		ID3D11DeviceContext *m_immediateContext;
		IDXGISwapChain *m_swapChain;
		ID3D11RenderTargetView *m_renderTargetView;
		ID3D11Texture2D *m_depthStencil;
		ID3D11DepthStencilView *m_depthStencilView;

		D3DPRESENT_PARAMETERS m_params;
		cViewport m_viewPort;

		//vector<sAlphaBlendSpace*> m_alphaSpace;
		//vector<sAlphaBlendSpace*> m_alphaSpaceBuffer;

		vector<sVertexDiffuse> m_grid;
		vector<sVertexDiffuse> m_axis;
		//cTextManager m_textMgr;

		ULONG_PTR gdiplusToken;

		// Display FPS 
		//cText m_textFps;
		float m_elapseTime;
		int m_fps;

		// Debug Render
		bool m_isDbgRender; // Debug Render
		int m_dbgRenderStyle; // 0:Sphere, 1:Box
		//cDbgBox m_dbgBox;
		//cDbgArrow m_dbgArrow;
		//cDbgSphere m_dbgSphere;
		//cDbgAxis m_dbgAxis;
	};


	// 렌더러 초기화.
	void ReleaseRenderer();
	inline ID3D11Device* cRenderer::GetDevice() { return m_d3dDevice; }
	inline ID3D11DeviceContext* cRenderer::GetDevContext() {return m_immediateContext;}
	inline HWND cRenderer::GetHwnd() const { return m_hWnd; }
}
