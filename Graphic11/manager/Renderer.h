//
// jjuiddong
// Renderer
//
#pragma once


namespace graphic
{
	struct sCbPerFrame
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProjection;
		XMVECTOR eyePosW;
	};

	class cRenderer
	{
	public:
		cRenderer();
		virtual ~cRenderer();

		bool CreateDirectX(const bool isImmediate, HWND hWnd, const int width, const int height
			, ID3D11Device *device = NULL);
		void Update(const float elpaseT);
		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDevContext();
		HWND GetHwnd() const;
		bool CheckResetDevice(const float width=0, const float height=0);
		bool ResetDevice(const float width=0, const float height=0, const bool forceReset=false
			, const bool resetResource=true);
		void AddRenderAlpha(cNode2 *node, const Vector3 &normal = Vector3(0, 0, 1), const Matrix44 &tm=Matrix44::Identity, const int opt= 1);
		void AddRenderAlpha(sAlphaBlendSpace *space, cNode2 *node, const Vector3 &normal = Vector3(0, 0, 1), const Matrix44 &tm = Matrix44::Identity, const int opt = 1);
		void AddAlphaBlendSpace(const cBoundingBox &bbox);
		sAlphaBlendSpace* GetCurrentAlphaBlendSpace();

		bool ClearScene();
		void BeginScene();
		void Present();
		void EndScene();
		void SetRenderTarget(ID3D11RenderTargetView *renderTargetView, ID3D11DepthStencilView *depthStencilView);
		void ExecuteCommandList(ID3D11CommandList *cmdList);
		void FinishCommandList();

		void RenderAxis();
		void RenderFPS();
		void RenderGrid();

		void SetLightEnable(const int light, const bool enable);

		//void SetCullMode(const D3DCULL cull);
		//void SetFillMode(const D3DFILLMODE mode);
		//void SetNormalizeNormals(const bool value);
		//void SetAlphaBlend(const bool value);
		//void SetZEnable(const bool value);
		//void SetZFunc(const D3DCMPFUNC value);
		//D3DFILLMODE GetFillMode();
		//D3DCULL GetCullMode();


	protected:
		void MakeGrid( const float width, const int count, DWORD color, vector<sVertexDiffuse> &out );
		void MakeAxis( const float length, DWORD xcolor, DWORD ycolor, DWORD zcolor, vector<sVertexDiffuse> &out );


	public:
		bool m_isImmediateMode; // immediate or deferred mode, default: true
		ID3D11Device *m_d3dDevice;

		HWND m_hWnd;
		ID3D11DeviceContext *m_devContext;
		IDXGISwapChain *m_swapChain;
		ID3D11RenderTargetView *m_renderTargetView;
		ID3D11Texture2D *m_depthStencil;
		ID3D11DepthStencilView *m_depthStencilView;
		ID3D11CommandList *m_cmdList;

		//D3DPRESENT_PARAMETERS m_params;
		cViewport m_viewPort;

		vector<sAlphaBlendSpace*> m_alphaSpace;
		vector<sAlphaBlendSpace*> m_alphaSpaceBuffer;

		cShaderManager m_shaderMgr;
		cFontManager m_fontMgr;
		cTextManager m_textMgr;

		cConstantBuffer<sCbPerFrame> m_cbPerFrame;
		cConstantBuffer<sCbLight> m_cbLight;
		cConstantBuffer<sCbMaterial> m_cbMaterial;

		vector<sVertexDiffuse> m_grid;
		vector<sVertexDiffuse> m_axis;

		// Display FPS 
		cText m_textFps;
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
	inline ID3D11DeviceContext* cRenderer::GetDevContext() {return m_devContext;}
	inline HWND cRenderer::GetHwnd() const { return m_hWnd; }
}
