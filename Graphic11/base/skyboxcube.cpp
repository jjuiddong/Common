
#include "stdafx.h"
#include "skyboxcube.h"


namespace graphic 
{
	const float terrain_geometry_scale = 1.2f;
	const int sky_gridpoints = 10;
	const float sky_texture_angle = 0.425f;
}

using namespace graphic;


cSkyBoxCube::cSkyBoxCube()
	: m_texture(NULL)
	, m_isDepthNone(false)
{
}

cSkyBoxCube::~cSkyBoxCube()
{
}


bool cSkyBoxCube::Create(cRenderer &renderer, const char *textureFileName)
{
	m_texture = cResourceManager::Get()->LoadTextureParallel(renderer, textureFileName);
	cResourceManager::Get()->AddParallelLoader(new cParallelLoader(cParallelLoader::eType::TEXTURE
		, textureFileName, (void**)&m_texture));

	CreateVertexBuffer(renderer);
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool cSkyBoxCube::CreateVertexBuffer(cRenderer &renderer)
{
	// creating sky vertex buffer
	const float r = 4000.0f;
	float *sky_vertexdata = new float[sky_gridpoints*(sky_gridpoints + 2) * 2 * 6];

	for (int j = 0; j<sky_gridpoints; j++)
	{
		int i = 0;
		int floatnum = (j*(sky_gridpoints + 2) * 2) * 6;
		sky_vertexdata[floatnum + 0] = terrain_gridpoints*terrain_geometry_scale*0.5f + r*cos(2.0f*MATH_PI*(float)i / (float)sky_gridpoints)*cos(-0.5f*MATH_PI + MATH_PI*(float)j / (float)sky_gridpoints);
		sky_vertexdata[floatnum + 1] = r*sin(-0.5f*MATH_PI + MATH_PI*(float)(j) / (float)sky_gridpoints);
		sky_vertexdata[floatnum + 2] = terrain_gridpoints*terrain_geometry_scale*0.5f + r*sin(2.0f*MATH_PI*(float)i / (float)sky_gridpoints)*cos(-0.5f*MATH_PI + MATH_PI*(float)j / (float)sky_gridpoints);
		sky_vertexdata[floatnum + 3] = 1;
		sky_vertexdata[floatnum + 4] = (sky_texture_angle + (float)i / (float)sky_gridpoints);
		sky_vertexdata[floatnum + 5] = 2.0f - 2.0f*(float)j / (float)sky_gridpoints;
		floatnum += 6;
		for (i = 0; i<sky_gridpoints + 1; i++)
		{
			sky_vertexdata[floatnum + 0] = terrain_gridpoints*terrain_geometry_scale*0.5f + r*cos(2.0f*MATH_PI*(float)i / (float)sky_gridpoints)*cos(-0.5f*MATH_PI + MATH_PI*(float)j / (float)sky_gridpoints);
			sky_vertexdata[floatnum + 1] = r*sin(-0.5f*MATH_PI + MATH_PI*(float)(j) / (float)sky_gridpoints);
			sky_vertexdata[floatnum + 2] = terrain_gridpoints*terrain_geometry_scale*0.5f + r*sin(2.0f*MATH_PI*(float)i / (float)sky_gridpoints)*cos(-0.5f*MATH_PI + MATH_PI*(float)j / (float)sky_gridpoints);
			sky_vertexdata[floatnum + 3] = 1;
			sky_vertexdata[floatnum + 4] = (sky_texture_angle + (float)i / (float)sky_gridpoints);
			sky_vertexdata[floatnum + 5] = 2.0f - 2.0f*(float)j / (float)sky_gridpoints;
			floatnum += 6;
			sky_vertexdata[floatnum + 0] = terrain_gridpoints*terrain_geometry_scale*0.5f + r*cos(2.0f*MATH_PI*(float)i / (float)sky_gridpoints)*cos(-0.5f*MATH_PI + MATH_PI*(float)(j + 1) / (float)sky_gridpoints);
			sky_vertexdata[floatnum + 1] = r*sin(-0.5f*MATH_PI + MATH_PI*(float)(j + 1) / (float)sky_gridpoints);
			sky_vertexdata[floatnum + 2] = terrain_gridpoints*terrain_geometry_scale*0.5f + r*sin(2.0f*MATH_PI*(float)i / (float)sky_gridpoints)*cos(-0.5f*MATH_PI + MATH_PI*(float)(j + 1) / (float)sky_gridpoints);
			sky_vertexdata[floatnum + 3] = 1;
			sky_vertexdata[floatnum + 4] = (sky_texture_angle + (float)i / (float)sky_gridpoints);
			sky_vertexdata[floatnum + 5] = 2.0f - 2.0f*(float)(j + 1) / (float)sky_gridpoints;
			floatnum += 6;
		}
		i = sky_gridpoints;
		sky_vertexdata[floatnum + 0] = terrain_gridpoints*terrain_geometry_scale*0.5f + r*cos(2.0f*MATH_PI*(float)i / (float)sky_gridpoints)*cos(-0.5f*MATH_PI + MATH_PI*(float)(j + 1) / (float)sky_gridpoints);
		sky_vertexdata[floatnum + 1] = r*sin(-0.5f*MATH_PI + MATH_PI*(float)(j + 1) / (float)sky_gridpoints);
		sky_vertexdata[floatnum + 2] = terrain_gridpoints*terrain_geometry_scale*0.5f + r*sin(2.0f*MATH_PI*(float)i / (float)sky_gridpoints)*cos(-0.5f*MATH_PI + MATH_PI*(float)(j + 1) / (float)sky_gridpoints);
		sky_vertexdata[floatnum + 3] = 1;
		sky_vertexdata[floatnum + 4] = (sky_texture_angle + (float)i / (float)sky_gridpoints);
		sky_vertexdata[floatnum + 5] = 2.0f - 2.0f*(float)(j + 1) / (float)sky_gridpoints;
		floatnum += 6;
	}

	m_vtxBuff.Create(renderer, sky_gridpoints*(sky_gridpoints + 2) * 2, sizeof(sVertexTex2), sky_vertexdata);
	m_vtxType = eVertexType::POSITION | eVertexType::TEXTURE0;

	free(sky_vertexdata);

	return true;
}


bool cSkyBoxCube::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = renderer.m_shaderMgr.FindShader("../media/shader11/skyboxcube.fxo");
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	Matrix44 view = GetMainCamera().GetViewMatrix();
	view._41 = 0;
	view._42 = -0.4f;
	view._43 = 0;

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(parentTm);
	renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(view.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbClipPlane.Update(renderer, 4);

	if (m_texture)
		m_texture->Bind(renderer, 4);

	m_vtxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	if (m_isDepthNone)
	{
		CommonStates states(renderer.GetDevice());
		renderer.GetDevContext()->OMSetDepthStencilState(states.DepthNone(), 0);
		renderer.GetDevContext()->Draw(sky_gridpoints*(sky_gridpoints + 2) * 2, 0);
		renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
	}
	else
	{
		renderer.GetDevContext()->Draw(sky_gridpoints*(sky_gridpoints + 2) * 2, 0);
	}

	// recovery camera constant buffer
	GetMainCamera().Bind(renderer);
	GetMainLight().Bind(renderer);

	// debugging
	++renderer.m_drawCallCount;
#ifdef _DEBUG
	++renderer.m_shadersDrawCall[m_vtxType];
#endif
	return true;
}


Matrix44 cSkyBoxCube::GetReflectMatrix()
{
	Matrix44 mWorld = GetMainCamera().GetViewMatrix().Inverse();
	mWorld._42 = -mWorld._42 - 1.0f;
	mWorld._21 *= -1.0f;
	mWorld._23 *= -1.0f;
	mWorld._32 *= -1.0f;

	Matrix44 view = GetMainCamera().GetViewMatrix();
	view._41 = 0;
	view._42 = -0.4f;
	view._43 = 0;
	
	const Matrix44 mReflect = mWorld.Inverse() * view.Inverse();
	return mReflect;
}
