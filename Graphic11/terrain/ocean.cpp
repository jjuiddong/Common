
#include "stdafx.h"
#include "ocean.h"


namespace graphic
{
	const int terrain_numpatches_1d = 64;
	const float terrain_geometry_scale = 1.2f;
	//const float terrain_geometry_scale = 2.f;
	const float terrain_maxheight = 30.0f;
	const float terrain_minheight = -30.0f;
	const float terrain_fractalfactor = 0.68f;
	const float terrain_fractalinitialvalue = 100.0f;
	const float terrain_smoothfactor1 = 0.99f;
	const float terrain_smoothfactor2 = 0.10f;
	const float terrain_rockfactor = 0.95f;
	const int terrain_smoothsteps = 40;

	const float terrain_far_range = terrain_gridpoints*terrain_geometry_scale;

	const int shadowmap_resource_buffer_size_xy = 4096;
	const int water_normalmap_resource_buffer_size_xy = 2048;
	//const int terrain_layerdef_map_texture_size = 1024;
	const int terrain_depth_shadow_map_texture_size = 512;

	const float main_buffer_size_multiplier = 1.1f;
	const float reflection_buffer_size_multiplier = 1.1f;
	const float refraction_buffer_size_multiplier = 1.1f;
}

using namespace graphic;


cOcean::cOcean()
	: m_shader(NULL)
	, m_TotalTime(0)
{
	m_microBumpTexScale[0] = 225;
	m_microBumpTexScale[1] = 225;
	m_waterBumpTexScale[0] = 14;
	m_waterBumpTexScale[1] = 14;
}

cOcean::~cOcean()
{
	Clear();
}


void cOcean::Create(cRenderer &renderer
	, const float backBufferWidth, const float backBufferHeight )
{
	m_MultiSampleCount = 4;
	m_MultiSampleQuality = 0;
	m_BackbufferWidth = backBufferWidth;
	m_BackbufferHeight = backBufferHeight;

    D3D11_INPUT_ELEMENT_DESC TerrainLayout[] =
        { "PATCH_PARAMETERS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	m_shader = renderer.m_shaderMgr.LoadShader(renderer, "../media/ocean5/Island11.fxo", TerrainLayout, 1);
	heightfield_inputlayout = m_shader->m_vtxLayout.m_vertexLayout;

	ID3D11Device *pDevice = renderer.GetDevice();
	ID3DX11Effect *pEffect = m_shader->m_effect;

	const D3D11_INPUT_ELEMENT_DESC SkyLayout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,   0, 16,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC passDesc;
	pEffect->GetTechniqueByName("MainToBackBuffer")->GetPassByIndex(0)->GetDesc(&passDesc);
	pDevice->CreateInputLayout(SkyLayout,
		2,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&trianglestrip_inputlayout);

	DirectX::CreateDDSTextureFromFile(renderer.GetDevice(), L"../media/TerrainTextures/water_bump.dds"
		, (ID3D11Resource**)&m_water_bump_texture, &m_water_bump_textureSRV);

	CreateTerrain(renderer);
	ReCreateBuffers(renderer);
}


void cOcean::ReCreateBuffers(cRenderer &renderer)
{
	ID3D11Device *pDevice = renderer.GetDevice();

	D3D11_TEXTURE2D_DESC tex_desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC textureSRV_desc;
	D3D11_DEPTH_STENCIL_VIEW_DESC DSV_desc;

	SAFE_RELEASE(m_main_color_resource);
	SAFE_RELEASE(m_main_color_resourceSRV);
	SAFE_RELEASE(m_main_color_resourceRTV);

	SAFE_RELEASE(m_main_color_resource_resolved);
	SAFE_RELEASE(m_main_color_resource_resolvedSRV);

	SAFE_RELEASE(m_main_depth_resource);
	SAFE_RELEASE(m_main_depth_resourceDSV);
	SAFE_RELEASE(m_main_depth_resourceSRV);

	SAFE_RELEASE(m_reflection_color_resource);
	SAFE_RELEASE(m_reflection_color_resourceSRV);
	SAFE_RELEASE(m_reflection_color_resourceRTV);
	SAFE_RELEASE(m_refraction_color_resource);
	SAFE_RELEASE(m_refraction_color_resourceSRV);
	SAFE_RELEASE(m_refraction_color_resourceRTV);

	SAFE_RELEASE(m_reflection_depth_resource);
	SAFE_RELEASE(m_reflection_depth_resourceDSV);
	SAFE_RELEASE(m_refraction_depth_resource);
	SAFE_RELEASE(m_refraction_depth_resourceSRV);
	SAFE_RELEASE(m_refraction_depth_resourceRTV);

	SAFE_RELEASE(m_shadowmap_resource);
	SAFE_RELEASE(m_shadowmap_resourceDSV);
	SAFE_RELEASE(m_shadowmap_resourceSRV);

	// recreating main color buffer

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(m_BackbufferWidth*main_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(m_BackbufferHeight*main_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count   = m_MultiSampleCount;
    tex_desc.SampleDesc.Quality = m_MultiSampleQuality;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;

    textureSRV_desc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	textureSRV_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D         ( &tex_desc, NULL, &m_main_color_resource );
    pDevice->CreateShaderResourceView( m_main_color_resource, &textureSRV_desc, &m_main_color_resourceSRV );
    pDevice->CreateRenderTargetView  ( m_main_color_resource, NULL, &m_main_color_resourceRTV );


	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(m_BackbufferWidth*main_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(m_BackbufferHeight*main_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;

    textureSRV_desc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D         ( &tex_desc, NULL, &m_main_color_resource_resolved );
    pDevice->CreateShaderResourceView( m_main_color_resource_resolved, &textureSRV_desc, &m_main_color_resource_resolvedSRV );

	// recreating main depth buffer

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(m_BackbufferWidth*main_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(m_BackbufferHeight*main_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R32_TYPELESS;
	tex_desc.SampleDesc.Count   = m_MultiSampleCount;
    tex_desc.SampleDesc.Quality = m_MultiSampleQuality;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;
	
	DSV_desc.Format  = DXGI_FORMAT_D32_FLOAT;
	DSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	DSV_desc.Flags = 0;
	DSV_desc.Texture2D.MipSlice = 0;

    textureSRV_desc.Format                    = DXGI_FORMAT_R32_FLOAT;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	textureSRV_desc.Texture2D.MipLevels		  = 1;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D( &tex_desc, NULL, &m_main_depth_resource );
 	pDevice->CreateDepthStencilView(m_main_depth_resource, &DSV_desc, &m_main_depth_resourceDSV );
	pDevice->CreateShaderResourceView( m_main_depth_resource, &textureSRV_desc, &m_main_depth_resourceSRV );

	// recreating reflection and refraction color buffers

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(m_BackbufferWidth*reflection_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(m_BackbufferHeight*reflection_buffer_size_multiplier);
    tex_desc.MipLevels          = (UINT)max(1,log(max((float)tex_desc.Width,(float)tex_desc.Height))/(float)log(2.0f));
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    textureSRV_desc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D         ( &tex_desc, NULL, &m_reflection_color_resource );
    pDevice->CreateShaderResourceView( m_reflection_color_resource, &textureSRV_desc, &m_reflection_color_resourceSRV );
    pDevice->CreateRenderTargetView  ( m_reflection_color_resource, NULL, &m_reflection_color_resourceRTV );


	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	tex_desc.Width              = (UINT)(m_BackbufferWidth*refraction_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(m_BackbufferHeight*refraction_buffer_size_multiplier);
    tex_desc.MipLevels          = (UINT)max(1,log(max((float)tex_desc.Width,(float)tex_desc.Height))/(float)log(2.0f));
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    textureSRV_desc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels = tex_desc.MipLevels;
	textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D         ( &tex_desc, NULL, &m_refraction_color_resource );
    pDevice->CreateShaderResourceView( m_refraction_color_resource, &textureSRV_desc, &m_refraction_color_resourceSRV );
    pDevice->CreateRenderTargetView  ( m_refraction_color_resource, NULL, &m_refraction_color_resourceRTV );

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	ZeroMemory(&tex_desc,sizeof(tex_desc));

	// recreating reflection and refraction depth buffers

	tex_desc.Width              = (UINT)(m_BackbufferWidth*reflection_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(m_BackbufferHeight*reflection_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R32_TYPELESS;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;
	
	DSV_desc.Format  = DXGI_FORMAT_D32_FLOAT;
	DSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSV_desc.Flags = 0;
	DSV_desc.Texture2D.MipSlice = 0;

	pDevice->CreateTexture2D( &tex_desc, NULL, &m_reflection_depth_resource );
 	pDevice->CreateDepthStencilView(m_reflection_depth_resource, &DSV_desc, &m_reflection_depth_resourceDSV );


	tex_desc.Width              = (UINT)(m_BackbufferWidth*refraction_buffer_size_multiplier);
    tex_desc.Height             = (UINT)(m_BackbufferHeight*refraction_buffer_size_multiplier);
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R32_FLOAT;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;
	
	textureSRV_desc.Format                    = DXGI_FORMAT_R32_FLOAT;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    textureSRV_desc.Texture2D.MipLevels       = 1;
    textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D( &tex_desc, NULL, &m_refraction_depth_resource );
 	pDevice->CreateRenderTargetView  (m_refraction_depth_resource, NULL, &m_refraction_depth_resourceRTV );
	pDevice->CreateShaderResourceView(m_refraction_depth_resource, &textureSRV_desc, &m_refraction_depth_resourceSRV );

	// recreating shadowmap resource
	tex_desc.Width              = shadowmap_resource_buffer_size_xy;
    tex_desc.Height             = shadowmap_resource_buffer_size_xy;
    tex_desc.MipLevels          = 1;
    tex_desc.ArraySize          = 1;
    tex_desc.Format             = DXGI_FORMAT_R32_TYPELESS;
    tex_desc.SampleDesc.Count   = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage              = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags     = 0;
    tex_desc.MiscFlags          = 0;
	
	DSV_desc.Format  = DXGI_FORMAT_D32_FLOAT;
	DSV_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSV_desc.Flags = 0;
	DSV_desc.Texture2D.MipSlice=0;

	textureSRV_desc.Format                    = DXGI_FORMAT_R32_FLOAT;
    textureSRV_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    textureSRV_desc.Texture2D.MipLevels       = 1;
    textureSRV_desc.Texture2D.MostDetailedMip = 0;

	pDevice->CreateTexture2D( &tex_desc, NULL, &m_shadowmap_resource);	
	pDevice->CreateShaderResourceView( m_shadowmap_resource, &textureSRV_desc, &m_shadowmap_resourceSRV );
	pDevice->CreateDepthStencilView(m_shadowmap_resource, &DSV_desc, &m_shadowmap_resourceDSV );
}

void cOcean::Clear()
{
	SAFE_RELEASE(m_water_bump_texture);
	SAFE_RELEASE(m_water_bump_textureSRV);

	SAFE_RELEASE(depthmap_texture);
	SAFE_RELEASE(depthmap_textureSRV);

	SAFE_RELEASE(m_main_color_resource);
	SAFE_RELEASE(m_main_color_resourceSRV);
	SAFE_RELEASE(m_main_color_resourceRTV);

	SAFE_RELEASE(m_main_color_resource_resolved);
	SAFE_RELEASE(m_main_color_resource_resolvedSRV);

	SAFE_RELEASE(m_main_depth_resource);
	SAFE_RELEASE(m_main_depth_resourceDSV);
	SAFE_RELEASE(m_main_depth_resourceSRV);

	SAFE_RELEASE(m_reflection_color_resource);
	SAFE_RELEASE(m_reflection_color_resourceSRV);
	SAFE_RELEASE(m_reflection_color_resourceRTV);
	SAFE_RELEASE(m_refraction_color_resource);
	SAFE_RELEASE(m_refraction_color_resourceSRV);
	SAFE_RELEASE(m_refraction_color_resourceRTV);

	SAFE_RELEASE(m_reflection_depth_resource);
	SAFE_RELEASE(m_reflection_depth_resourceDSV);
	SAFE_RELEASE(m_refraction_depth_resource);
	SAFE_RELEASE(m_refraction_depth_resourceRTV);
	SAFE_RELEASE(m_refraction_depth_resourceSRV);

	SAFE_RELEASE(m_shadowmap_resource);
	SAFE_RELEASE(m_shadowmap_resourceDSV);
	SAFE_RELEASE(m_shadowmap_resourceSRV);

	SAFE_RELEASE(trianglestrip_inputlayout);

	SAFE_RELEASE(heightfield_vertexbuffer);
	//SAFE_RELEASE(heightfield_inputlayout);
}

int gp_wrap(int a)
{
	if (a<0) return (a + terrain_gridpoints);
	if (a >= terrain_gridpoints) return (a - terrain_gridpoints);
	return a;
}

float bilinear_interpolation(float fx, float fy, float a, float b, float c, float d)
{
	float s1, s2, s3, s4;
	s1 = fx*fy;
	s2 = (1 - fx)*fy;
	s3 = (1 - fx)*(1 - fy);
	s4 = fx*(1 - fy);
	return((a*s3 + b*s4 + c*s1 + d*s2));
}

void cOcean::CreateTerrain(cRenderer &renderer)
{
	ID3D11Device *pDevice = renderer.GetDevice();

	int i, j;
	float * backterrain;
	Vector3 vec1,vec2,vec3;
	int currentstep=terrain_gridpoints;
	float mv,rm;
	float offset=0,yscale=0,maxheight=0,minheight=0;

	float *patches_rawdata;
	HRESULT result;
	D3D11_SUBRESOURCE_DATA subresource_data;
	D3D11_TEXTURE2D_DESC tex_desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC textureSRV_desc; 

	backterrain = (float *) malloc((terrain_gridpoints+1)*(terrain_gridpoints+1)*sizeof(float));
	rm=terrain_fractalinitialvalue;
	backterrain[0]=0;
	backterrain[0+terrain_gridpoints*terrain_gridpoints]=0;
	backterrain[terrain_gridpoints]=0;
	backterrain[terrain_gridpoints+terrain_gridpoints*terrain_gridpoints]=0;
    currentstep=terrain_gridpoints;
	srand(12);

	for (int i = 0; i < (terrain_gridpoints + 1)*(terrain_gridpoints + 1); ++i)
		backterrain[i] = -40.f +(float)common::randfloat2() * -10;

	// scaling to minheight..maxheight range
	for (i=0;i<terrain_gridpoints+1;i++)
		for (j=0;j<terrain_gridpoints+1;j++)
		{
			m_height[i][j]=backterrain[i+terrain_gridpoints*j];
		}
	maxheight=m_height[0][0];
	minheight=m_height[0][0];
	for(i=0;i<terrain_gridpoints+1;i++)
		for(j=0;j<terrain_gridpoints+1;j++)
		{
			if(m_height[i][j]>maxheight) maxheight=m_height[i][j];
			if(m_height[i][j]<minheight) minheight=m_height[i][j];
		}
	offset=minheight-terrain_minheight;
	yscale=(terrain_maxheight-terrain_minheight)/(maxheight-minheight);

	for(i=0;i<terrain_gridpoints+1;i++)
		for(j=0;j<terrain_gridpoints+1;j++)
		{
			m_height[i][j]-=minheight;
			m_height[i][j]*=yscale;
			m_height[i][j]+=terrain_minheight;
		}

	// moving down edges of heightmap	
	for (i=0;i<terrain_gridpoints+1;i++)
		for (j=0;j<terrain_gridpoints+1;j++)
		{
			mv=(float)((i-terrain_gridpoints/2.0f)*(i-terrain_gridpoints/2.0f)+(j-terrain_gridpoints/2.0f)*(j-terrain_gridpoints/2.0f));
			rm=(float)((terrain_gridpoints*0.8f)*(terrain_gridpoints*0.8f)/4.0f);
			if(mv>rm)
			{
				m_height[i][j]-=((mv-rm)/1000.0f)*terrain_geometry_scale;
			}
			if(m_height[i][j]<terrain_minheight)
			{
				m_height[i][j]=terrain_minheight;
			}
		}	

	free(backterrain);

	patches_rawdata = new float [terrain_numpatches_1d*terrain_numpatches_1d*4];

	//building depthmap
	byte * depth_shadow_map_texture_pixels=(byte *)malloc(terrain_depth_shadow_map_texture_size*terrain_depth_shadow_map_texture_size*4);
	for (i = 0; i<terrain_depth_shadow_map_texture_size; i++)
		for (j = 0; j<terrain_depth_shadow_map_texture_size; j++)
		{
			float x = (float)(terrain_gridpoints)*((float)i / (float)terrain_depth_shadow_map_texture_size);
			float z = (float)(terrain_gridpoints)*((float)j / (float)terrain_depth_shadow_map_texture_size);
			int ix = (int)floor(x);
			int iz = (int)floor(z);
			rm = bilinear_interpolation(x - ix, z - iz, m_height[ix][iz], m_height[ix + 1][iz], m_height[ix + 1][iz + 1], m_height[ix][iz + 1])*terrain_geometry_scale;

			if (rm>0)
			{
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size + i) * 4 + 0] = 0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size + i) * 4 + 1] = 0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size + i) * 4 + 2] = 0;
			}
			else
			{
				float no = (1.0f*255.0f*(rm / (terrain_minheight*terrain_geometry_scale))) - 1.0f;
				if (no>255) no = 255;
				if (no<0) no = 0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size + i) * 4 + 0] = (byte)no;

				no = (10.0f*255.0f*(rm / (terrain_minheight*terrain_geometry_scale))) - 40.0f;
				if (no>255) no = 255;
				if (no<0) no = 0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size + i) * 4 + 1] = (byte)no;

				no = (100.0f*255.0f*(rm / (terrain_minheight*terrain_geometry_scale))) - 300.0f;
				if (no>255) no = 255;
				if (no<0) no = 0;
				depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size + i) * 4 + 2] = (byte)no;
			}
			depth_shadow_map_texture_pixels[(j*terrain_depth_shadow_map_texture_size + i) * 4 + 3] = 0;
		}


	subresource_data.pSysMem = depth_shadow_map_texture_pixels;
	subresource_data.SysMemPitch = terrain_depth_shadow_map_texture_size*4;
	subresource_data.SysMemSlicePitch = 0;

	tex_desc.Width = terrain_depth_shadow_map_texture_size;
	tex_desc.Height = terrain_depth_shadow_map_texture_size;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count = 1; 
	tex_desc.SampleDesc.Quality = 0; 
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;
	result=pDevice->CreateTexture2D(&tex_desc,&subresource_data,&depthmap_texture);

	ZeroMemory(&textureSRV_desc,sizeof(textureSRV_desc));
	textureSRV_desc.Format=tex_desc.Format;
	textureSRV_desc.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
	textureSRV_desc.Texture2D.MipLevels=tex_desc.MipLevels;
	pDevice->CreateShaderResourceView(depthmap_texture,&textureSRV_desc,&depthmap_textureSRV);

	free(depth_shadow_map_texture_pixels);

	// creating terrain vertex buffer
	for(int i=0;i<terrain_numpatches_1d;i++)
		for(int j=0;j<terrain_numpatches_1d;j++)
		{
			patches_rawdata[(i+j*terrain_numpatches_1d)*4+0]=i*terrain_geometry_scale*terrain_gridpoints/terrain_numpatches_1d;
			patches_rawdata[(i+j*terrain_numpatches_1d)*4+1]=j*terrain_geometry_scale*terrain_gridpoints/terrain_numpatches_1d;
			patches_rawdata[(i+j*terrain_numpatches_1d)*4+2]=terrain_geometry_scale*terrain_gridpoints/terrain_numpatches_1d;
			patches_rawdata[(i+j*terrain_numpatches_1d)*4+3]=terrain_geometry_scale*terrain_gridpoints/terrain_numpatches_1d;
		}

	D3D11_BUFFER_DESC buf_desc;
	memset(&buf_desc,0,sizeof(buf_desc));

	buf_desc.ByteWidth = terrain_numpatches_1d*terrain_numpatches_1d*4*sizeof(float);
	buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buf_desc.Usage = D3D11_USAGE_DEFAULT;

	subresource_data.pSysMem=patches_rawdata;
	subresource_data.SysMemPitch=0;
	subresource_data.SysMemSlicePitch=0;

	result=pDevice->CreateBuffer(&buf_desc,&subresource_data,&heightfield_vertexbuffer);
	free (patches_rawdata);
}


float g_LightPosition[3] = { -10000.0f,6500.0f,10000.0f };

void cOcean::Render(cRenderer &renderer, cCamera *cam, const float deltaSeconds)
{
	ID3D11Device *pDevice = renderer.GetDevice();
	ID3DX11Effect *pEffect = m_shader->m_effect;

	ID3D11DeviceContext* pContext;
	ID3DX11EffectShaderResourceVariable* tex_variable;
	float origin[2]={0,0};
	UINT stride=sizeof(float)*4;
	UINT offset=0;
	UINT cRT = 1;

	pDevice->GetImmediateContext(&pContext);

	m_TotalTime += deltaSeconds;
	Vector2 WaterTexcoordShift(m_TotalTime*1.5f, m_TotalTime*0.75f);
	Vector2 ScreenSizeInv(1.0f / (m_BackbufferWidth*main_buffer_size_multiplier), 1.0f / (m_BackbufferHeight*main_buffer_size_multiplier));

	ID3DX11Effect *effect = m_shader->m_effect;
	effect->GetVariableByName("g_ZNear")->AsScalar()->SetFloat(scene_z_near);
	effect->GetVariableByName("g_ZFar")->AsScalar()->SetFloat(scene_z_far);
	effect->GetVariableByName("g_LightPosition")->AsVector()->SetFloatVector(g_LightPosition);
	effect->GetVariableByName("g_WaterBumpTexcoordShift")->AsVector()->SetFloatVector((float*)&WaterTexcoordShift);
	effect->GetVariableByName("g_ScreenSizeInv")->AsVector()->SetFloatVector((float*)&ScreenSizeInv);

	effect->GetVariableByName("g_DynamicTessFactor")->AsScalar()->SetFloat(m_DynamicTessellationFactor);
	effect->GetVariableByName("g_StaticTessFactor")->AsScalar()->SetFloat(m_StaticTessellationFactor);
	effect->GetVariableByName("g_UseDynamicLOD")->AsScalar()->SetFloat(m_UseDynamicLOD ? 1.0f : 0.0f);
	effect->GetVariableByName("g_RenderCaustics")->AsScalar()->SetFloat(m_RenderCaustics ? 1.0f : 0.0f);
	effect->GetVariableByName("g_FrustumCullInHS")->AsScalar()->SetFloat(m_FrustumCullInHS ? 1.0f : 0.0f);
	
	effect->GetVariableByName("g_WaterMicroBumpTexcoordScale")->AsVector()->SetFloatVector(m_microBumpTexScale);
	effect->GetVariableByName("g_WaterBumpTexcoordScale")->AsVector()->SetFloatVector(m_waterBumpTexScale);

	tex_variable=pEffect->GetVariableByName("g_WaterBumpTexture")->AsShaderResource();
	tex_variable->SetResource(m_water_bump_textureSRV);

	//tex_variable=pEffect->GetVariableByName("g_DepthMapTexture")->AsShaderResource();
	//tex_variable->SetResource(depthmap_textureSRV);

	pEffect->GetVariableByName("g_HeightFieldOrigin")->AsVector()->SetFloatVector(origin);
	pEffect->GetVariableByName("g_HeightFieldSize")->AsScalar()->SetFloat(terrain_gridpoints*terrain_geometry_scale);
	

	//ID3D11RenderTargetView *colorBuffer = renderer.m_renderTargetView;
	//ID3D11DepthStencilView  *backBuffer = renderer.m_depthStencilView;
	ID3D11RenderTargetView *colorBuffer = NULL;
	ID3D11DepthStencilView *backBuffer = NULL;
	//renderer.GetDevContext()->OMGetRenderTargets(1, &colorBuffer, &backBuffer);

	D3D11_VIEWPORT currentViewport;
	D3D11_VIEWPORT reflection_Viewport;
	D3D11_VIEWPORT refraction_Viewport;
	D3D11_VIEWPORT shadowmap_resource_viewport;
	D3D11_VIEWPORT water_normalmap_resource_viewport;
	D3D11_VIEWPORT main_Viewport;

    float ClearColor[4] = { 0.8f, 0.8f, 1.0f, 1.0f };
	float RefractionClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

	reflection_Viewport.Width=(float)m_BackbufferWidth*reflection_buffer_size_multiplier;
	reflection_Viewport.Height=(float)m_BackbufferHeight*reflection_buffer_size_multiplier;
	reflection_Viewport.MaxDepth=1;
	reflection_Viewport.MinDepth=0;
	reflection_Viewport.TopLeftX=0;
	reflection_Viewport.TopLeftY=0;

	refraction_Viewport.Width=(float)m_BackbufferWidth*refraction_buffer_size_multiplier;
	refraction_Viewport.Height=(float)m_BackbufferHeight*refraction_buffer_size_multiplier;
	refraction_Viewport.MaxDepth=1;
	refraction_Viewport.MinDepth=0;
	refraction_Viewport.TopLeftX=0;
	refraction_Viewport.TopLeftY=0;

	main_Viewport.Width=(float)m_BackbufferWidth*main_buffer_size_multiplier;
	main_Viewport.Height=(float)m_BackbufferHeight*main_buffer_size_multiplier;
	main_Viewport.MaxDepth=1;
	main_Viewport.MinDepth=0;
	main_Viewport.TopLeftX=0;
	main_Viewport.TopLeftY=0;

	shadowmap_resource_viewport.Width=shadowmap_resource_buffer_size_xy;
	shadowmap_resource_viewport.Height=shadowmap_resource_buffer_size_xy;
	shadowmap_resource_viewport.MaxDepth=1;
	shadowmap_resource_viewport.MinDepth=0;
	shadowmap_resource_viewport.TopLeftX=0;
	shadowmap_resource_viewport.TopLeftY=0;

	water_normalmap_resource_viewport.Width=water_normalmap_resource_buffer_size_xy;
	water_normalmap_resource_viewport.Height=water_normalmap_resource_buffer_size_xy;
	water_normalmap_resource_viewport.MaxDepth=1;
	water_normalmap_resource_viewport.MinDepth=0;
	water_normalmap_resource_viewport.TopLeftX=0;
	water_normalmap_resource_viewport.TopLeftY=0;

	//saving scene color buffer and back buffer to constants
	pContext->RSGetViewports( &cRT, &currentViewport);
    pContext->OMGetRenderTargets( 1, &colorBuffer, &backBuffer );

	// selecting shadowmap_resource rendertarget
	pContext->RSSetViewports(1,&shadowmap_resource_viewport);
    pContext->OMSetRenderTargets( 0, NULL, m_shadowmap_resourceDSV);
    pContext->ClearDepthStencilView( m_shadowmap_resourceDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

	pEffect->GetTechniqueByName("Default")->GetPassByIndex(0)->Apply(0, pContext);

	// setting up reflection rendertarget	
	pContext->RSSetViewports(1,&reflection_Viewport);
    pContext->OMSetRenderTargets( 1, &m_reflection_color_resourceRTV, m_reflection_depth_resourceDSV);
    pContext->ClearRenderTargetView( m_reflection_color_resourceRTV, RefractionClearColor );
    pContext->ClearDepthStencilView( m_reflection_depth_resourceDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

	tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);

	pEffect->GetTechniqueByName("RenderHeightfield")->GetPassByIndex(0)->Apply(0, pContext);

	// setting up main rendertarget	
	pContext->RSSetViewports(1,&main_Viewport);
    pContext->OMSetRenderTargets( 1, &m_main_color_resourceRTV, m_main_depth_resourceDSV);
    pContext->ClearRenderTargetView( m_main_color_resourceRTV, ClearColor );
    pContext->ClearDepthStencilView( m_main_depth_resourceDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );
	SetupNormalView(cam);

	tex_variable=pEffect->GetVariableByName("g_WaterNormalMapTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);

	tex_variable=pEffect->GetVariableByName("g_WaterNormalMapTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);
	pEffect->GetTechniqueByName("RenderHeightfield")->GetPassByIndex(0)->Apply(0, pContext);

	// resolving main buffer color to refraction color resource
	pContext->ResolveSubresource(m_refraction_color_resource,0,m_main_color_resource,0,DXGI_FORMAT_R8G8B8A8_UNORM);

	// resolving main buffer depth to refraction depth resource manually
    pContext->RSSetViewports( 1, &main_Viewport );
	pContext->OMSetRenderTargets( 1, &m_refraction_depth_resourceRTV, NULL);
	float depthClear[4] = { 1,1,1,1 };
	pContext->ClearRenderTargetView(m_refraction_depth_resourceRTV, depthClear); // add test code


	// getting back to rendering to main buffer 
	//pContext->RSSetViewports(1,&main_Viewport);
 //   pContext->OMSetRenderTargets( 1, &m_main_color_resourceRTV, m_main_depth_resourceDSV);

	pContext->OMSetRenderTargets(1, &colorBuffer, backBuffer);
	pContext->RSSetViewports(1, &currentViewport);

	// drawing water surface to main buffer
	tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	//tex_variable->SetResource(shadowmap_resourceSRV);
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_ReflectionTexture")->AsShaderResource();
	tex_variable->SetResource(m_reflection_color_resourceSRV);
	tex_variable=pEffect->GetVariableByName("g_RefractionTexture")->AsShaderResource();
	//tex_variable->SetResource(refraction_color_resourceSRV);
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureResolved")->AsShaderResource();
	tex_variable->SetResource(m_refraction_depth_resourceSRV);
	tex_variable=pEffect->GetVariableByName("g_WaterNormalMapTexture")->AsShaderResource();
	tex_variable->SetResource(NULL); // add test code

	pEffect->GetVariableByName("g_TerrainBeingRendered")->AsScalar()->SetFloat(0.0f);
	pEffect->GetTechniqueByName("RenderWater")->GetPassByIndex(0)->Apply(0, pContext);

	pContext->IASetInputLayout(heightfield_inputlayout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	stride=sizeof(float)*4;
	pContext->IASetVertexBuffers(0,1,&heightfield_vertexbuffer,&stride,&offset);
	pContext->Draw(terrain_numpatches_1d*terrain_numpatches_1d, 0);

    SAFE_RELEASE ( colorBuffer );
    SAFE_RELEASE ( backBuffer );

	pContext->Release();
}


void cOcean::SetupReflectionView(graphic::cCamera *cam)
{
	ID3DX11Effect *pEffect = m_shader->m_effect;
	float aspectRatio = m_BackbufferWidth / m_BackbufferHeight;

	Vector3 EyePoint;
	Vector3 LookAtPoint;

	EyePoint = cam->GetEyePos();
	LookAtPoint = cam->GetLookAt();
	EyePoint.y=-1.0f*EyePoint.y+1.0f;
	LookAtPoint.y=-1.0f*LookAtPoint.y+1.0f;

	Matrix44 mView;
	Matrix44 mProj;
	Matrix44 mViewProj;
	Matrix44 mViewProjInv;

	Matrix44 mWorld;
	mView = cam->GetViewMatrix();
	mWorld = cam->GetViewMatrix().Inverse();

	mWorld._42=-mWorld._42-1.0f;
	
	mWorld._21*=-1.0f;
	mWorld._23*=-1.0f;

	mWorld._32*=-1.0f;
	
	mView = mWorld.Inverse();
	mProj.SetProjection(camera_fov * MATH_PI / 360.0f, aspectRatio, scene_z_near, scene_z_far);
	mViewProj=mView*mProj;
	mViewProjInv = mViewProj.Inverse();

	pEffect->GetVariableByName("g_ModelViewProjectionMatrix")->AsMatrix()->SetMatrix((float*)&mViewProj);
	pEffect->GetVariableByName("g_CameraPosition")->AsVector()->SetFloatVector((float*)&EyePoint);
	Vector3 direction = LookAtPoint - EyePoint;
	Vector3 normalized_direction = direction.Normal();
	pEffect->GetVariableByName("g_CameraDirection")->AsVector()->SetFloatVector((float*)&normalized_direction);

	pEffect->GetVariableByName("g_HalfSpaceCullSign")->AsScalar()->SetFloat(1.0f);
	pEffect->GetVariableByName("g_HalfSpaceCullPosition")->AsScalar()->SetFloat(-0.6f);
}

void cOcean::SetupRefractionView(graphic::cCamera *cam)
{
	ID3DX11Effect *pEffect = m_shader->m_effect;
	pEffect->GetVariableByName("g_HalfSpaceCullSign")->AsScalar()->SetFloat(-1.0f);
	pEffect->GetVariableByName("g_HalfSpaceCullPosition")->AsScalar()->SetFloat(terrain_minheight);
}


void cOcean::SetupNormalView(graphic::cCamera *cam)
{
	ID3DX11Effect *pEffect = m_shader->m_effect;

	Vector3 EyePoint;
	Vector3 LookAtPoint;

	EyePoint = cam->GetEyePos();
	LookAtPoint = cam->GetLookAt();
	Matrix44 mView = cam->GetViewMatrix();
	Matrix44 mProjMatrix = cam->GetProjectionMatrix();
	Matrix44 mViewProj = mView * mProjMatrix;
	Matrix44 mViewProjInv;
	mViewProjInv = mViewProj.Inverse();
	Vector3 cameraPosition = cam->GetEyePos();

	pEffect->GetVariableByName("g_ModelViewMatrix")->AsMatrix()->SetMatrix((float*)&mView);
	pEffect->GetVariableByName("g_ModelViewProjectionMatrix")->AsMatrix()->SetMatrix((float*)&mViewProj);
	pEffect->GetVariableByName("g_ModelViewProjectionMatrixInv")->AsMatrix()->SetMatrix((float*)&mViewProjInv);
	pEffect->GetVariableByName("g_CameraPosition")->AsVector()->SetFloatVector((float*)&cameraPosition);
	Vector3 direction = LookAtPoint - EyePoint;
	Vector3 normalized_direction = direction.Normal();
	pEffect->GetVariableByName("g_CameraDirection")->AsVector()->SetFloatVector((float*)&normalized_direction);
	pEffect->GetVariableByName("g_HalfSpaceCullSign")->AsScalar()->SetFloat(1.0);
	pEffect->GetVariableByName("g_HalfSpaceCullPosition")->AsScalar()->SetFloat(terrain_minheight*2);
}


void cOcean::ChangeWindowSize(cRenderer &renderer, const float backBufferWidth, const float backBufferHeight)
{
	m_BackbufferWidth = backBufferWidth;
	m_BackbufferHeight = backBufferHeight;
	ReCreateBuffers(renderer);
}
