//
// jjuiddong
// Renderer
//
#pragma once


namespace graphic
{

	class cRenderer// : public common::cSingleton<cRenderer>
	{
	public:
		cRenderer();
		virtual ~cRenderer();

		bool CreateDirectX(HWND hWnd, const int width, const int height);
		void Update(const float elpaseT);
		LPDIRECT3DDEVICE9 GetDevice();
		HWND GetHwnd() const;
		bool CheckResetDevice(const int width, const int height);
		bool ResetDevice(const int width, const int height);
		void AddPostRender(iRenderable *obj, const int opt=0);


		bool ClearScene();
		void BeginScene();
		void Present();
		void EndScene();

		void RenderAxis();
		void RenderFPS();
		void RenderGrid();

		void SetCullMode(const D3DCULL cull);
		void SetFillMode(const D3DFILLMODE mode);
		void SetNormalizeNormals(const bool value);
		void SetAlphaBlend(const bool value);
		void SetZEnable(const bool value);

		D3DFILLMODE GetFillMode();


	protected:
		void MakeGrid( const float width, const int count, DWORD color, vector<sVertexDiffuse> &out );
		void MakeAxis( const float length, DWORD xcolor, DWORD ycolor, DWORD zcolor, vector<sVertexDiffuse> &out );


	public:
		HWND m_hWnd;
		LPDIRECT3DDEVICE9 m_pDevice;
		D3DPRESENT_PARAMETERS m_params;
		int m_width;
		int m_height;

		struct sRenderObj
		{
			int opt;
			iRenderable *obj;
		};
		vector<sRenderObj> m_postRender;
		vector<sVertexDiffuse> m_grid;
		vector<sVertexDiffuse> m_axis;

		// Display FPS 
		cText m_textFps;
		float m_elapseTime;
		int m_fps;
	};


	// 렌더러 초기화.
	void ReleaseRenderer();
	inline LPDIRECT3DDEVICE9 cRenderer::GetDevice() { return m_pDevice; }
	inline HWND cRenderer::GetHwnd() const { return m_hWnd; }
}
