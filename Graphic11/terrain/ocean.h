// Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com
//
// 2017-10-18, jjuiddong
// - modify nvidia Island example
//
#pragma once

#define terrain_gridpoints 512
//#define terrain_gridpoints 1024
#define scene_z_near 1.0f
#define scene_z_far 25000.0f
#define camera_fov 110.0f


namespace graphic
{
	class cOcean
	{
	public:
		cOcean();
		virtual ~cOcean();

		void Create(cRenderer &renderer, const float backBufferWidth, const float backBufferHeight);
		void ReCreateBuffers(cRenderer &renderer);
		void Render(cRenderer &renderer, cCamera3D *cam, cSkyBox *skyBox, const float deltaSeconds);
		void CreateTerrain(cRenderer &renderer);

		void SetupNormalView(cCamera3D *);
		void SetupReflectionView(cCamera3D *);
		void SetupRefractionView(cCamera3D *);
		void ChangeWindowSize(cRenderer &renderer, const float backBufferWidth, const float backBufferHeight);
		Plane GetWorldPlane();
		void Clear();


	public:
		float m_BackbufferWidth;
		float m_BackbufferHeight;
		UINT m_MultiSampleCount;
		UINT m_MultiSampleQuality;

		cTexture *m_waterBump;
		cTexture m_depthMap;
		cRenderTarget m_reflection;
		cRenderTarget m_refraction;
		cRenderTarget m_refractionDepth;
		cRenderTarget m_mainColor;
		cDepthBuffer m_shadowMap;

		float m_height[terrain_gridpoints + 1][terrain_gridpoints + 1];

		cVertexBuffer m_heightField;
		ID3D11InputLayout *heightfield_inputlayout;

		cShader11 *m_shader;
		float m_TotalTime;
		float m_DynamicTessellationFactor = 50.0f;
		float m_StaticTessellationFactor = 12.0f;
		bool m_UseDynamicLOD = true;
		bool m_RenderCaustics = true;
		bool m_FrustumCullInHS = true;

		float m_microBumpTexScale[3];
		float m_waterBumpTexScale[3];
	};

}
