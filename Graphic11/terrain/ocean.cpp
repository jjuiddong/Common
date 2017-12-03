
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

	const int sky_gridpoints = 10;
	const float sky_texture_angle = 0.425f;

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
	//m_waterBumpTexScale[0] = 14;
	//m_waterBumpTexScale[1] = 14;
	m_waterBumpTexScale[0] = 7;
	m_waterBumpTexScale[1] = 7;
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

	m_waterBump = cResourceManager::Get()->LoadTextureParallel(renderer, "../media/TerrainTextures/water_bump.dds");
	cResourceManager::Get()->AddParallelLoader(new cParallelLoader(cParallelLoader::eType::TEXTURE
		, "../media/TerrainTextures/water_bump.dds", (void**)&m_waterBump));

	CreateTerrain(renderer);
	ReCreateBuffers(renderer);
}


void cOcean::ReCreateBuffers(cRenderer &renderer)
{
	cViewport vp;
	vp.Create(0, 0, (m_BackbufferWidth*main_buffer_size_multiplier), (m_BackbufferHeight*main_buffer_size_multiplier), 0, 1);
	m_mainColor.Create(renderer, vp, DXGI_FORMAT_R8G8B8A8_UNORM, true, true, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT);

	vp.Create(0, 0, (m_BackbufferWidth*reflection_buffer_size_multiplier), (m_BackbufferHeight*reflection_buffer_size_multiplier), 0, 1);
	m_reflection.Create(renderer, vp, DXGI_FORMAT_R8G8B8A8_UNORM, false, true, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT);

	vp.Create(0, 0, (m_BackbufferWidth*refraction_buffer_size_multiplier), (m_BackbufferHeight*refraction_buffer_size_multiplier), 0, 1);
	m_refraction.Create(renderer, vp, DXGI_FORMAT_R8G8B8A8_UNORM, false, true, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT);
	m_refractionDepth.Create(renderer, vp, DXGI_FORMAT_R8G8B8A8_UNORM, false, false);

	cViewport depthVp;
	depthVp.Create(0, 0, (float)shadowmap_resource_buffer_size_xy, (float)shadowmap_resource_buffer_size_xy, 0, 1);
	m_shadowMap.Create(renderer, depthVp, false, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT);
}

void cOcean::Clear()
{
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

	float *patches_rawdata = new float [terrain_numpatches_1d*terrain_numpatches_1d*4];

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

	m_depthMap.Create(renderer, terrain_depth_shadow_map_texture_size, terrain_depth_shadow_map_texture_size
		, DXGI_FORMAT_R8G8B8A8_UNORM, depth_shadow_map_texture_pixels, terrain_depth_shadow_map_texture_size * 4);

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

	m_heightField.Create(renderer, terrain_numpatches_1d*terrain_numpatches_1d, sizeof(Vector4), patches_rawdata);
	free (patches_rawdata);
}


float g_LightPosition[3] = { -10000.0f,6500.0f,10000.0f };

void cOcean::Render(cRenderer &renderer
	, cCamera3D *cam
	, cSkyBox *skyBox
	, const float deltaSeconds)
{
	ID3D11Device *pDevice = renderer.GetDevice();
	ID3DX11Effect *pEffect = m_shader->m_effect;

	ID3D11DeviceContext* pContext;
	ID3DX11EffectShaderResourceVariable* tex_variable;
	float origin[2]={0,0};
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
	if (m_waterBump)
		tex_variable->SetResource(m_waterBump->m_texSRV);

	//tex_variable=pEffect->GetVariableByName("g_DepthMapTexture")->AsShaderResource();
	//tex_variable->SetResource(depthmap_textureSRV);

	pEffect->GetVariableByName("g_HeightFieldOrigin")->AsVector()->SetFloatVector(origin);
	pEffect->GetVariableByName("g_HeightFieldSize")->AsScalar()->SetFloat(terrain_gridpoints*terrain_geometry_scale);
	
	ID3D11RenderTargetView *colorBuffer = NULL;
	ID3D11DepthStencilView *backBuffer = NULL;

    Vector4 ClearColor(0.8f, 0.8f, 1.0f, 1.0f);
	Vector4 RefractionClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	D3D11_VIEWPORT reflection_Viewport;
	reflection_Viewport.Width=(float)m_BackbufferWidth*reflection_buffer_size_multiplier;
	reflection_Viewport.Height=(float)m_BackbufferHeight*reflection_buffer_size_multiplier;
	reflection_Viewport.MaxDepth=1;
	reflection_Viewport.MinDepth=0;
	reflection_Viewport.TopLeftX=0;
	reflection_Viewport.TopLeftY=0;

	D3D11_VIEWPORT refraction_Viewport;
	refraction_Viewport.Width=(float)m_BackbufferWidth*refraction_buffer_size_multiplier;
	refraction_Viewport.Height=(float)m_BackbufferHeight*refraction_buffer_size_multiplier;
	refraction_Viewport.MaxDepth=1;
	refraction_Viewport.MinDepth=0;
	refraction_Viewport.TopLeftX=0;
	refraction_Viewport.TopLeftY=0;

	D3D11_VIEWPORT main_Viewport;
	main_Viewport.Width=(float)m_BackbufferWidth*main_buffer_size_multiplier;
	main_Viewport.Height=(float)m_BackbufferHeight*main_buffer_size_multiplier;
	main_Viewport.MaxDepth=1;
	main_Viewport.MinDepth=0;
	main_Viewport.TopLeftX=0;
	main_Viewport.TopLeftY=0;

	D3D11_VIEWPORT shadowmap_resource_viewport;
	shadowmap_resource_viewport.Width=shadowmap_resource_buffer_size_xy;
	shadowmap_resource_viewport.Height=shadowmap_resource_buffer_size_xy;
	shadowmap_resource_viewport.MaxDepth=1;
	shadowmap_resource_viewport.MinDepth=0;
	shadowmap_resource_viewport.TopLeftX=0;
	shadowmap_resource_viewport.TopLeftY=0;

	//saving scene color buffer and back buffer to constants
	D3D11_VIEWPORT currentViewport;
	pContext->RSGetViewports( &cRT, &currentViewport);
    pContext->OMGetRenderTargets( 1, &colorBuffer, &backBuffer );

	// selecting shadowmap_resource rendertarget
	m_shadowMap.m_viewPort = shadowmap_resource_viewport;
	m_shadowMap.Begin(renderer, true);

	// setting up reflection rendertarget	
	m_reflection.m_viewPort = reflection_Viewport;
	m_reflection.BeginSimple(renderer, RefractionClearColor);

	if (skyBox)
		skyBox->Render(renderer, skyBox->GetReflectMatrix().GetMatrixXM());

	//tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	//tex_variable->SetResource(NULL);

	// setting up main rendertarget	
	m_mainColor.m_viewPort = main_Viewport;
	m_mainColor.BeginSimple(renderer, ClearColor);
	SetupNormalView(cam);

	tex_variable=pEffect->GetVariableByName("g_WaterNormalMapTexture")->AsShaderResource();
	tex_variable->SetResource(NULL);
	//tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	//tex_variable->SetResource(NULL);

	// resolving main buffer color to refraction color resource
	pContext->ResolveSubresource(m_refraction.m_texture, 0, m_mainColor.m_texture, 0, DXGI_FORMAT_R8G8B8A8_UNORM);

	// resolving main buffer depth to refraction depth resource manually
	m_refractionDepth.m_viewPort = main_Viewport;
	m_refractionDepth.BeginSimple(renderer, Vector4(1,1,1,1), true);

	// getting back to rendering to main buffer 
	pContext->OMSetRenderTargets(1, &colorBuffer, backBuffer);
	pContext->RSSetViewports(1, &currentViewport);

	// drawing water surface to main buffer
	tex_variable=pEffect->GetVariableByName("g_DepthTexture")->AsShaderResource();
	//tex_variable->SetResource(shadowmap_resourceSRV);
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_ReflectionTexture")->AsShaderResource();
	tex_variable->SetResource(m_reflection.m_texSRV);
	tex_variable=pEffect->GetVariableByName("g_RefractionTexture")->AsShaderResource();
	//tex_variable->SetResource(refraction_color_resourceSRV);
	tex_variable->SetResource(NULL);
	tex_variable=pEffect->GetVariableByName("g_RefractionDepthTextureResolved")->AsShaderResource();
	tex_variable->SetResource(m_refractionDepth.m_texSRV);
	tex_variable=pEffect->GetVariableByName("g_WaterNormalMapTexture")->AsShaderResource();
	tex_variable->SetResource(NULL); // add test code

	pEffect->GetVariableByName("g_TerrainBeingRendered")->AsScalar()->SetFloat(0.0f);
	pEffect->GetTechniqueByName("RenderWater")->GetPassByIndex(0)->Apply(0, pContext);

	pContext->IASetInputLayout(heightfield_inputlayout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	m_heightField.Bind(renderer);
	pContext->Draw(terrain_numpatches_1d*terrain_numpatches_1d, 0);

	SAFE_RELEASE ( colorBuffer );
    SAFE_RELEASE ( backBuffer );
	pContext->Release();
}


void cOcean::SetupReflectionView(graphic::cCamera3D *cam)
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
	mViewProj = mView * mProj;
	mViewProjInv = mViewProj.Inverse();

	pEffect->GetVariableByName("g_ModelViewProjectionMatrix")->AsMatrix()->SetMatrix((float*)&mViewProj);
	pEffect->GetVariableByName("g_CameraPosition")->AsVector()->SetFloatVector((float*)&EyePoint);
	Vector3 direction = LookAtPoint - EyePoint;
	Vector3 normalized_direction = direction.Normal();
	pEffect->GetVariableByName("g_CameraDirection")->AsVector()->SetFloatVector((float*)&normalized_direction);

	pEffect->GetVariableByName("g_HalfSpaceCullSign")->AsScalar()->SetFloat(1.0f);
	pEffect->GetVariableByName("g_HalfSpaceCullPosition")->AsScalar()->SetFloat(-0.6f);
}

void cOcean::SetupRefractionView(graphic::cCamera3D *cam)
{
	ID3DX11Effect *pEffect = m_shader->m_effect;
	pEffect->GetVariableByName("g_HalfSpaceCullSign")->AsScalar()->SetFloat(-1.0f);
	pEffect->GetVariableByName("g_HalfSpaceCullPosition")->AsScalar()->SetFloat(terrain_minheight);
}


void cOcean::SetupNormalView(graphic::cCamera3D *cam)
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


Plane cOcean::GetWorldPlane()
{
	// Reflection plane in local space.
	Plane waterPlaneL(0, -1, 0, 0);

	// Reflection plane in world space.
	Matrix44 waterWorld;
	waterWorld.SetTranslate(Vector3(0, 0, 0)); // water height
	Matrix44 WInvTrans;
	WInvTrans = waterWorld.Inverse();
	WInvTrans.Transpose();
	Plane waterPlaneW = waterPlaneL * WInvTrans;
	return waterPlaneW;
}