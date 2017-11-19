
#include "common.fx"

//#define SHADOW_EPSILON 0.001f


//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
    float2 Tex : TEXCOORD0;
    float3 toEye : TEXCOORD1;
	float clip : SV_ClipDistance0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float3 Tangent : TANGENT
	, float3 Binormal : BINORMAL
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

    output.Pos = mul( Pos, mWorld );
    output.Pos = mul( output.Pos, gView );
    output.Pos = mul( output.Pos, gProjection );
    output.Normal = normalize( mul(Normal, (float3x3)mWorld) );
	output.Tangent = normalize(mul(Tangent, (float3x3)mWorld));
	output.Binormal = normalize(mul(Binormal, (float3x3)mWorld));
    output.Tex = Tex;
	output.clip = dot(mul(Pos, mWorld), gClipPlane);

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT In ) : SV_Target
{
	float4 bumpMap = txBump.Sample(samAnis, In.Tex);
	// Expand the range of the normal value from (0, +1) to (-1, +1). 
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the bump map.
	float3 bumpNormal = In.Normal + bumpMap.x * In.Tangent + bumpMap.y * In.Binormal;

	// Normalize the resulting bump normal.
	bumpNormal = normalize(bumpNormal);

	float4 color = GetLightingColor(bumpNormal, In.toEye, 1.f);
	float4 texColor = txDiffuse.Sample(samLinear, In.Tex);
	float4 Out = color * texColor;
	return float4(Out.xyz, gMtrl_Diffuse.a * texColor.a);
}



//--------------------------------------------------------------------------------------
// Vertex Shader ShadowMap
//--------------------------------------------------------------------------------------
struct VS_SHADOW_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float2 Tex : TEXCOORD0;
	float4 TexShadow0 : TEXCOORD1;
	float4 TexShadow1 : TEXCOORD2;
	float4 TexShadow2 : TEXCOORD3;
	float3 toEye : TEXCOORD4;
	float4 Depth0 : TEXCOORD5;
	float2 Depth1 : TEXCOORD6;
	float clip : SV_ClipDistance0;
};

VS_SHADOW_OUTPUT VS_ShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, float3 Tangent : TANGENT
	, float3 Binormal : BINORMAL
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	output.Pos = mul(Pos, mWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Normal = normalize(mul(Normal, (float3x3)mWorld));
	output.Tangent = normalize(mul(Tangent, (float3x3)mWorld));
	output.Binormal = normalize(mul(Binormal, (float3x3)mWorld));
	output.Tex = Tex;

	matrix mLVP[3];
	matrix mVPT[3];

	float4 wPos = mul(Pos, mWorld);
	mLVP[0] = mul(gLightView[0], gLightProj[0]);
	mVPT[0] = mul(mLVP[0], gLightTT);
	mLVP[1] = mul(gLightView[1], gLightProj[1]);
	mVPT[1] = mul(mLVP[1], gLightTT);
	mLVP[2] = mul(gLightView[2], gLightProj[2]);
	mVPT[2] = mul(mLVP[2], gLightTT);

	output.TexShadow0 = mul(wPos, mVPT[0]);
	output.TexShadow1 = mul(wPos, mVPT[1]);
	output.TexShadow2 = mul(wPos, mVPT[2]);

	output.Depth0.xy = mul(wPos, mLVP[0]).zw;
	output.Depth0.zw = mul(wPos, mLVP[1]).zw;
	output.Depth1.xy = mul(wPos, mLVP[2]).zw;

	output.clip = dot(mul(Pos, mWorld), gClipPlane);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader ShadowMap
//--------------------------------------------------------------------------------------
float4 PS_ShadowMap(VS_SHADOW_OUTPUT In) : SV_Target
{
	const float fShadowTerm = GetShadowPCF(In.Depth0, In.Depth1
	, In.TexShadow0.xy, In.TexShadow1.xy, In.TexShadow2.xy);

	float4 bumpMap = txBump.Sample(samAnis, In.Tex);
	// Expand the range of the normal value from (0, +1) to (-1, +1). 
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the bump map.
	float3 bumpNormal = In.Normal + bumpMap.x * In.Tangent + bumpMap.y * In.Binormal;

	// Normalize the resulting bump normal.
	bumpNormal = normalize(bumpNormal);

	float4 color = GetLightingColor(bumpNormal, In.toEye, fShadowTerm*0.9f);
	float4 texColor = txDiffuse.Sample(samLinear, In.Tex);
	float4 Out = color * texColor;
	return float4(Out.xyz, gMtrl_Diffuse.a * texColor.a);
}


//--------------------------------------------------------------------------------------
// Vertex Shader BuildShadowMap
//--------------------------------------------------------------------------------------
struct VS_BUILDSHADOW_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Depth : TEXCOORD0;
};


VS_BUILDSHADOW_OUTPUT VS_BuildShadowMap(float4 Pos : POSITION
	, float3 Normal : NORMAL
	, float2 Tex : TEXCOORD0
	, uint instID : SV_InstanceID
	, uniform bool IsInstancing
)
{
	VS_BUILDSHADOW_OUTPUT output = (VS_BUILDSHADOW_OUTPUT)0;
	const matrix mWorld = IsInstancing ? gWorldInst[instID] : gWorld;

	output.Pos = mul(Pos, mWorld);
	output.Pos = mul(output.Pos, gView);
	output.Pos = mul(output.Pos, gProjection);
	output.Depth.xy = output.Pos.zw;

	return output;
}


float4 PS_BuildShadowMap(VS_BUILDSHADOW_OUTPUT In) : SV_Target
{
	clip(gMtrl_Diffuse.a - 0.5f);
	return float4(1, 1, 1, 1);
}


//----------------------------------------------------------


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


technique11 ShadowMap_Instancing
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_ShadowMap(Instancing)));
		SetGeometryShader(NULL);
        SetHullShader(NULL);
      	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ShadowMap()));
	}
}


technique11 BuildShadowMap_Instancing
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_BuildShadowMap(Instancing)));
		SetGeometryShader(NULL);
        SetHullShader(NULL);
       	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_BuildShadowMap()));
	}
}
