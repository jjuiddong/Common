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

		void Create(cRenderer &renderer
			, const float backBufferWidth, const float backBufferHeight);
		void ReCreateBuffers(cRenderer &renderer);
		void Render(cRenderer &renderer
			, cCamera *cam
			, cSkyBox *skyBox
			, const float deltaSeconds);
		void CreateTerrain(cRenderer &renderer);

		void SetupNormalView(cCamera *);
		void SetupReflectionView(cCamera *);
		void SetupRefractionView(cCamera *);
		void ChangeWindowSize(cRenderer &renderer, const float backBufferWidth, const float backBufferHeight);
		void Clear();


	public:
		float m_BackbufferWidth;
		float m_BackbufferHeight;
		UINT m_MultiSampleCount;
		UINT m_MultiSampleQuality;

		ID3D11Texture2D		*m_water_bump_texture;
		ID3D11ShaderResourceView *m_water_bump_textureSRV;

		ID3D11Texture2D		*sky_texture;
		ID3D11ShaderResourceView *sky_textureSRV;

		ID3D11Texture2D			 *m_reflection_color_resource;
		ID3D11ShaderResourceView *m_reflection_color_resourceSRV;
		ID3D11RenderTargetView   *m_reflection_color_resourceRTV;
		ID3D11Texture2D			 *m_refraction_color_resource;
		ID3D11ShaderResourceView *m_refraction_color_resourceSRV;
		ID3D11RenderTargetView   *m_refraction_color_resourceRTV;

		ID3D11Texture2D			 *m_shadowmap_resource;
		ID3D11ShaderResourceView *m_shadowmap_resourceSRV;
		ID3D11DepthStencilView   *m_shadowmap_resourceDSV;

		ID3D11Texture2D			 *m_reflection_depth_resource;
		ID3D11DepthStencilView   *m_reflection_depth_resourceDSV;

		ID3D11Texture2D			 *m_refraction_depth_resource;
		ID3D11RenderTargetView   *m_refraction_depth_resourceRTV;
		ID3D11ShaderResourceView *m_refraction_depth_resourceSRV;

		ID3D11Texture2D			 *m_main_color_resource;
		ID3D11ShaderResourceView *m_main_color_resourceSRV;
		ID3D11RenderTargetView   *m_main_color_resourceRTV;
		ID3D11Texture2D			 *m_main_depth_resource;
		ID3D11DepthStencilView   *m_main_depth_resourceDSV;
		ID3D11ShaderResourceView *m_main_depth_resourceSRV;
		ID3D11Texture2D			 *m_main_color_resource_resolved;
		ID3D11ShaderResourceView *m_main_color_resource_resolvedSRV;

		float m_height[terrain_gridpoints + 1][terrain_gridpoints + 1];

		ID3D11Texture2D		*depthmap_texture;
		ID3D11ShaderResourceView *depthmap_textureSRV;
		ID3D11Buffer		*heightfield_vertexbuffer;
		ID3D11Buffer		*sky_vertexbuffer;
		ID3D11InputLayout   *heightfield_inputlayout;
		ID3D11InputLayout   *trianglestrip_inputlayout;

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
