
#include "common.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_POSDIFFUSE VS( float4 Pos : POSITION
	, float4 Color : COLOR 
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VSOUT_POSDIFFUSE output = (VSOUT_POSDIFFUSE)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	output.Pos = mul( Pos, mWorld );
    output.Pos = mul( output.Pos, gView );
    output.Pos = mul( output.Pos, gProjection );
    output.Color = Color;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VSOUT_POSDIFFUSE In ) : SV_Target
{
	return In.Color * gMtrl_Diffuse;
}


//--------------------------------------------------------------------------------------
// Vertex Shader ShadowMap
//--------------------------------------------------------------------------------------
VSOUT_SHADOW VS_ShadowMap(float4 Pos : POSITION
	, float4 Color : COLOR
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VSOUT_SHADOW output = (VSOUT_SHADOW)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	float4 PosW = mul(Pos, mWorld);
	output.Pos = mul(PosW, gView);
	output.Pos = mul(output.Pos, gProjection);

	matrix mLVP[3];
	matrix mVPT[3];

	mLVP[0] = mul(gLightView[0], gLightProj[0]);
	mVPT[0] = mul(mLVP[0], gLightTT);
	mLVP[1] = mul(gLightView[1], gLightProj[1]);
	mVPT[1] = mul(mLVP[1], gLightTT);
	mLVP[2] = mul(gLightView[2], gLightProj[2]);
	mVPT[2] = mul(mLVP[2], gLightTT);

	output.TexShadow0 = mul(PosW, mVPT[0]);
	output.TexShadow1 = mul(PosW, mVPT[1]);
	output.TexShadow2 = mul(PosW, mVPT[2]);

	output.Depth0.xy = mul(PosW, mLVP[0]).zw;
	output.Depth0.zw = mul(PosW, mLVP[1]).zw;
	output.Depth1.xy = mul(PosW, mLVP[2]).zw;

	output.Color = Color;
	output.clip = dot(PosW, gClipPlane);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader ShadowMap
//--------------------------------------------------------------------------------------
float4 PS_ShadowMap(VSOUT_SHADOW In) : SV_Target
{
	const float fShadowTerm = GetShadowPCF(In.Depth0, In.Depth1
	, In.TexShadow0.xy, In.TexShadow1.xy, In.TexShadow2.xy);
	float4 Out = In.Color * gMtrl_Diffuse * fShadowTerm;
	return float4(Out.xyz, gMtrl_Diffuse.a);
}


//--------------------------------------------------------------------------------------
// Vertex Shader BuildShadowMap
//--------------------------------------------------------------------------------------
VSOUT_BUILDSHADOW VS_BuildShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VSOUT_BUILDSHADOW output = (VSOUT_BUILDSHADOW)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	output.Pos = mul(Pos, mWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Depth.xy = output.Pos.zw;

	return output;
}


float4 PS_BuildShadowMap(VSOUT_BUILDSHADOW In) : SV_Target
{
	clip(gMtrl_Diffuse.a - 0.5f);
	return float4(1, 1, 1, 1);
}



//--------------------------------------------------------------------------------------

technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS(NotInstancing)));
		SetGeometryShader(NULL);
        SetHullShader(NULL);
       	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 ShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_ShadowMap(NotInstancing)));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ShadowMap()));
	}
}


technique11 BuildShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_BuildShadowMap(NotInstancing)));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_BuildShadowMap()));
	}
}


//---------------------------------------------------------------------------------
// Instancing

technique11 Unlit_Instancing
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS(Instancing)));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
