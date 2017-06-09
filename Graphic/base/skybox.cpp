#include "stdafx.h"
#include "skybox.h"


using namespace graphic;


cSkyBox::cSkyBox()
{
	ZeroMemory(m_textures, sizeof(m_textures));
}

cSkyBox::~cSkyBox()
{
}


// textureFilePath : 이 파일 경로에 skybox_top, skybox_front, skybox_back, 
//				skybox_left, skybox_right, skybox_bottom.jpg 파일이 있어야 한다.
bool cSkyBox::Create(cRenderer &renderer, const string &textureFilePath)
{
	string textureFileName[] = 
	{
		//"skybox_front.jpg", "skybox_back.jpg", "skybox_left.jpg", 
		//"skybox_right.jpg", "skybox_top.jpg", "skybox_bottom.jpg"

		"ThickCloudsWaterFront2048.png", 
		"ThickCloudsWaterBack2048.png",
		"ThickCloudsWaterRight2048.png",
		"ThickCloudsWaterLeft2048.png",
		"ThickCloudsWaterUp2048.png",
		"ThickCloudsWaterDown2048.png",

		//"blueclear2_front.png", 
		//"blueclear2_back.png",
		//"blueclear2_left.png",
		//"blueclear2_right.png",
		//"blueclear2_top.png",
		//"blueclear2_bottom.png",
	};

	for (int i=0; i < MAX_FACE; ++i)
	{
		const string fileName = textureFilePath + "/" + textureFileName[ i];
		//m_textures[i] = cResourceManager::Get()->LoadTexture(renderer, fileName);
		m_textures[i] = cResourceManager::Get()->LoadTextureParallel(renderer, fileName);
		cResourceManager::Get()->AddParallelLoader(new cParallelLoader(cParallelLoader::eType::TEXTURE, fileName, (void**)&m_textures[i]) );
	}

	if (!CreateVertexBuffer(renderer))
		return false;

	m_shader = cResourceManager::Get()->LoadShader(renderer, "cube3.fx");
	if (!m_shader)
		return false;
	m_shader->SetTechnique("SkyTech");

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool  cSkyBox::CreateVertexBuffer(cRenderer &renderer)
{
	// Example diagram of "front" quad
	// The numbers are vertices
	// 
	// 2  __________ 4
	//   |\         |
	//	 |  \       |
	//   |    \     |
	//   |      \   |
	// 1 |        \ | 3
	//	  ----------	 
	const float size = 300;
	sVertexTex SkyboxMesh[24] =
	{
		// Front quad, NOTE: All quads face inward
		sVertexTex(-size, -size,  size,  0.0f, 1.0f ),
		sVertexTex(-size,  size,  size,  0.0f, 0.0f ),
		sVertexTex( size, -size,  size,  1.0f, 1.0f ),
		sVertexTex( size,  size,  size,  1.0f, 0.0f ),

		// Back quad
		sVertexTex( size, -size, -size,  0.0f, 1.0f ),
		sVertexTex( size,  size, -size,  0.0f, 0.0f ),
		sVertexTex(-size, -size, -size,  1.0f, 1.0f ),
		sVertexTex(-size,  size, -size,  1.0f, 0.0f ),

		// Left quad
		sVertexTex(-size, -size, -size,  0.0f, 1.0f ),
		sVertexTex(-size,  size, -size,  0.0f, 0.0f ),
		sVertexTex(-size, -size,  size,  1.0f, 1.0f ),
		sVertexTex(-size,  size,  size,  1.0f, 0.0f ),

		// Right quad
		sVertexTex( size, -size,  size,  0.0f, 1.0f ),
		sVertexTex( size,  size,  size,  0.0f, 0.0f ),
		sVertexTex( size, -size, -size,  1.0f, 1.0f ),
		sVertexTex( size,  size, -size,  1.0f, 0.0f ),

		// Top quad
		sVertexTex(-size,  size,  size,  0.0f, 1.0f ),
		sVertexTex(-size,  size, -size,  0.0f, 0.0f ),
		sVertexTex( size,  size,  size,  1.0f, 1.0f ),
		sVertexTex( size,  size, -size,  1.0f, 0.0f ),

		// Bottom quad
		sVertexTex(-size, -size, -size,  0.0f, 1.0f ),
		sVertexTex(-size, -size,  size,  0.0f, 0.0f ),
		sVertexTex( size, -size, -size,  1.0f, 1.0f ),
		sVertexTex( size, -size,  size,  1.0f, 0.0f ),
	};

	const int vtxSize = 24;
	m_vtxBuff.Create(renderer, vtxSize, sizeof(sVertexTex), sVertexTex::FVF);

	sVertexTex *pv = (sVertexTex*)m_vtxBuff.Lock();
	memcpy( pv, SkyboxMesh, sizeof(sVertexTex) * 24 );
	m_vtxBuff.Unlock();

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void cSkyBox::Render(cRenderer &renderer, const Matrix44 &tm)
{
	renderer.SetCullMode(D3DCULL_NONE);
	renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	//renderer.GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	renderer.GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);


	DWORD lighting, fogEnable;
	renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lighting);
	renderer.GetDevice()->GetRenderState(D3DRS_FOGENABLE, &fogEnable);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	renderer.GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	renderer.GetDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	//mat matView, matViewSave, matWorld;
	Matrix44 matView, matViewSave, matWorld;
	renderer.GetDevice()->GetTransform(D3DTS_VIEW, (D3DXMATRIX*)&matViewSave);
	matView = matViewSave;
	matView._41 = 0.0f; matView._42 = -0.4f; matView._43 = 0.0f;
	renderer.GetDevice()->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&matView);
	// Set a default world matrix
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&tm);

	// render
	m_vtxBuff.Bind(renderer);
	renderer.GetDevice()->SetTexture(0, NULL);

	for (int i = 0 ; i < MAX_FACE; i++)
	{
		if (m_textures[i])
			m_textures[ i]->Bind(renderer, 0);
		renderer.GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
	}

	renderer.GetDevice()->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&matViewSave);

	renderer.SetCullMode(D3DCULL_CCW);
	renderer.GetDevice()->SetTexture(0, NULL);
	renderer.GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//renderer.GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lighting);
	renderer.GetDevice()->SetRenderState(D3DRS_FOGENABLE, fogEnable);
}


void cSkyBox::RenderShader(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_shader);
	// Nothing~
}
