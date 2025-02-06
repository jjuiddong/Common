//
// jjuiddong
// Renderer
//
#pragma once

#include "RendererInterface.h"


namespace graphic
{
	using namespace common;

	struct sCbPerFrame
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProjection;
		XMMATRIX mLightView[3];
		XMMATRIX mLightProj[3];
		XMMATRIX mLightTT;
		XMVECTOR eyePosW;
		XMVECTOR fogColor; // fog color r,g,b, fog distance rcp
		XMVECTOR outlineColor; // outline r,g,b
	};
	
	struct sCbInstancing
	{
		XMMATRIX worlds[256];
		XMVECTOR diffuses[256];
	};

	struct sCbClipPlane
	{
		float clipPlane[4];
		float reflectAlpha[4];
	};

	struct sCbSkinning
	{
		XMMATRIX mPalette[64];
	};

	struct sTessellationBuffer
	{
		float tessellationAmount;
		Vector2 size; // quad size
		float level;
		Vector4 vtxDir[2]; // lonLat vector2 x 2
		float edgeLevel[4]; // west-north-east-south, adjacent quad level
		float tuvs[4]; // texture uv coordinate
		float huvs[4]; // heightmap uv coordinate
	};


	class cTexture;
	class cDepthBuffer;
	class cRenderList;

	class cRenderer : public iRenderer
	{
	public:
		cRenderer();
		virtual ~cRenderer();

		bool CreateDirectX(const bool isThreadRender, HWND hWnd
			, const float width, const float height );
		bool CreateDirectXSubRenderer(const bool isThreadRender, HWND hWnd
			, const float width, const float height
			, ID3D11Device *device, ID3D11DeviceContext *devContext);

		void Update(const float elpaseT);
		virtual ID3D11Device* GetDevice() override;
		virtual ID3D11DeviceContext* GetDevContext() override;
		HWND GetHwnd() const;
		bool CheckResetDevice(const float width=0, const float height=0);
		bool ResetDevice(const float width=0, const float height=0, const bool forceReset=false
			, const bool resetResource=true);
		void AddRenderAlphaAll(cNode *node, const XMMATRIX &parentTm = XMIdentity);
		void AddRenderAlpha(cNode *node, const int opt = 1);
		void AddRenderAlpha(sAlphaBlendSpace *space, cNode *node, const int opt = 1);
		sAlphaBlendSpace* PushAlphaBlendSpace(const cBoundingBox &bbox);
		void PopAlphaBlendSpace();
		sAlphaBlendSpace* GetCurrentAlphaBlendSpace();

		bool ClearScene(const bool updateRenderTarget = true
			, const Vector4 &color=Vector4(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.0f));
		void BeginScene();
		void Present();
		void EndScene();
		void SetRenderTarget(ID3D11RenderTargetView *renderTargetView, ID3D11DepthStencilView *depthStencilView);
		void SetRenderTargetDepth(ID3D11DepthStencilView *depthStencilView);
		void ExecuteCommandList(ID3D11CommandList *cmdList);
		void FinishCommandList();
		void ReadConfig(const StrPath &fileName);

		void BindTexture(cTexture *texture, const int stage);
		void BindTexture(cRenderTarget &rt, const int stage);
		void BindTexture(cDepthBuffer &db, const int stage);
		void UnbindTexture(const int stage);
		void UnbindTextureAll();
		void UnbindShaderAll();

		void RenderAxis();
		void RenderAxis2();
		void RenderFPS();
		void RenderGrid();


	protected:
		void InitRenderer(HWND hWnd, const float width, const float height);


	public:
		bool m_isMainRenderer;
		bool m_isThreadRender; // immediate or deferred mode, default: false
		ID3D11Device *m_d3dDevice; // thread mode (deferred mode) is reference

		HWND m_hWnd;
		ID3D11DeviceContext *m_devContext;
		IDXGISwapChain *m_swapChain;
		ID3D11RenderTargetView *m_renderTargetView; // backbuffer
		ID3D11DepthStencilView *m_depthStencilView;
		ID3D11RenderTargetView *m_refRTV; // reference
		ID3D11DepthStencilView *m_refDSV; // reference
		ID3D11Texture2D *m_backBuffer;
		ID3D11CommandList *m_cmdList;

		cViewport m_viewPort;

		vector<sAlphaBlendSpace*> m_renderAlphaSpace;
		vector<sAlphaBlendSpace*> m_alphaSpaceStack; // stack
		vector<sAlphaBlendSpace*> m_alphaSpaceBuffer; // buffer
		uint m_alphaSpaceBufferCount;
		cMaterial m_defaultMtrl;

		CommonStates m_renderState;
		cShaderManager m_shaderMgr;
		cFontManager m_fontMgr;
		cTextManager m_textMgr;
		cRenderList *m_renderList;

		enum {MAX_TEXTURE_STAGE=10, TEXTURE_OFFSET=4};
		// Diffuse
		// Normal
		// Specular
		// Emissive
		// Texture1 - m_textureMap[0]
		// Texture2 - m_textureMap[1]
		// Texture3 - m_textureMap[2]
		// Texture4 - m_textureMap[3]
		// ...
		ID3D11ShaderResourceView *m_textureMap[MAX_TEXTURE_STAGE]; // reference

		cConstantBuffer<sCbPerFrame> m_cbPerFrame;
		cConstantBuffer<sCbLight> m_cbLight;
		cConstantBuffer<sCbMaterial> m_cbMaterial;
		cConstantBuffer<sCbInstancing> m_cbInstancing;
		cConstantBuffer<sCbClipPlane> m_cbClipPlane;
		cConstantBuffer<sCbSkinning> m_cbSkinning;
		graphic::cConstantBuffer<sTessellationBuffer> m_cbTessellation;
		
		// Display FPS 
		cText m_textFps;
		float m_elapseTime;
		int m_fps;

		// Debug Render
		bool m_isDbgRender; // Debug Render
		int m_dbgRenderStyle; // 0:Sphere, 1:Box, 2:None
		int m_textGenerateCount; // cTextManager generate count
		int m_textCacheCount; // cTextManager cache count
		int m_drawCallCount;
		int m_preDrawCallCount;
		map<int, int> m_shadersDrawCall; // key = vertex type, value = count
		map<int, int> m_preShadersDrawCall; // key = vertex type, value = count

		cCube m_dbgCube;
		cCube m_dbgCube2;
		cDbgBox m_dbgBox;
		cDbgBoxLine m_dbgBoxLine;
		cDbgLine m_dbgLine;
		cDbgArrow m_dbgArrow;
		cDbgSphere m_dbgSphere;
		cDbgAxis m_dbgAxis;
		cDbgAxis m_dbgAxis2; // short y axis
		cLine m_line;
		cLine2D m_line2D;
		cRect2D m_rect2D;
		cRect3D m_rect3D;
		cSphere m_sphere;
		cCylinder m_cylinder;
		cCapsule m_capsule;
		cCone m_cone;
	};


	// 렌더러 초기화.
	void ReleaseRenderer();
	inline ID3D11Device* cRenderer::GetDevice() { return m_d3dDevice; }
	inline ID3D11DeviceContext* cRenderer::GetDevContext() {return m_devContext;}
	inline HWND cRenderer::GetHwnd() const { return m_hWnd; }
}
